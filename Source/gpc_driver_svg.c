/********************************************************************
    FILE: gpc_driver_svg.c
    (c) 1984-2026 Scientific Endeavors Corporation / Modernized Port
    PURPOSE: Native Scalable Vector Graphics (SVG) Driver for GraphiC.
             Directly implements the f_tableq() driver interface,
             producing crisp, resolution-independent modern SVG vector
             files and companion interactive HTML5 pan/zoom viewers.
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphic.h>

#if defined(_WIN32)
#include <direct.h>
#define gpc_mkdir(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#define gpc_mkdir(dir) mkdir(dir, 0755)
#endif

static void ensure_parent_dirs(const char *path) {
	char temp[1024];
	char *p;
	if (!path || strlen(path) >= sizeof(temp)) return;
	strncpy(temp, path, sizeof(temp));
	for (p = temp + 1; *p; p++) {
		if (*p == '/' || *p == '\\') {
			if (p > temp && p[-1] == ':') {
				/* Skip Windows drive specifier like C:\ */
				continue;
			}
			char sep = *p;
			*p = '\0';
			gpc_mkdir(temp);
			*p = sep;
		}
	}
}

static FILE *svg_fp = NULL;
static FILE *svg_tmp_fp = NULL;
static char current_svg_path[512] = "graphic_plot.svg";
static char current_html_path[512] = "graphic_plot.html";
static int svg_page_num = 0;
static int svg_in_plot = 0;
static int canvas_w = MAXRESX;   /* 8192 */
static int canvas_h = MAXRESY;   /* 6266 */
static int max_y_coord = MAXRESYMO; /* 6265 */

/* Auto-crop settings and bounding box state */
static int svg_autocrop_enabled = 1;
static float svg_crop_margin = 0.25f; /* default 1/4 inch */
static int svg_has_drawn = 0;
static int svg_bbox_min_x = 0;
static int svg_bbox_min_y = 0;
static int svg_bbox_max_x = 0;
static int svg_bbox_max_y = 0;
static char svg_bg_color_hex[16] = "";
static int svg_has_bg = 0;

/* External color converter from Color.c */
extern void TYPE GRCtoIBM(int i, int rgb[3], int *IBMcolor);

/********************************************************************/
/* Set SVG plot auto-cropping margin in inches                      */
/* margin >= 0: enable cropping with specified margin (default 0.25) */
/* margin < 0: disable cropping, output full letter sheet           */
/********************************************************************/
void gpc_set_svg_crop(float margin_inches)
{
	if (margin_inches < 0.0f) {
		svg_autocrop_enabled = 0;
		svg_crop_margin = -1.0f;
	} else {
		svg_autocrop_enabled = 1;
		svg_crop_margin = margin_inches;
	}
}

/********************************************************************/
/* Update plot bounding box for a point with stroke width allowance */
/********************************************************************/
static void svg_update_bbox_pt(int x, int y, int half_stroke)
{
	int x0 = x - half_stroke;
	int x1 = x + half_stroke;
	int y0 = y - half_stroke;
	int y1 = y + half_stroke;

	if (!svg_has_drawn) {
		svg_bbox_min_x = x0;
		svg_bbox_max_x = x1;
		svg_bbox_min_y = y0;
		svg_bbox_max_y = y1;
		svg_has_drawn = 1;
	} else {
		if (x0 < svg_bbox_min_x) svg_bbox_min_x = x0;
		if (x1 > svg_bbox_max_x) svg_bbox_max_x = x1;
		if (y0 < svg_bbox_min_y) svg_bbox_min_y = y0;
		if (y1 > svg_bbox_max_y) svg_bbox_max_y = y1;
	}
}

/********************************************************************/
/* Convert GraphiC color number to #RRGGBB                          */
/********************************************************************/
static void color_to_svg_hex(int color_num, char *hex_out, size_t max_len)
{
	int rgb_63[3];
	int ibm_c;
	int r, g, b;

	if ((color_num & 0xFF000000) == 0x01000000) {
		r = (color_num >> 16) & 0xFF;
		g = (color_num >> 8) & 0xFF;
		b = color_num & 0xFF;
		snprintf(hex_out, max_len, "#%02X%02X%02X", r, g, b);
		return;
	}

	if (color_num == WHITE || color_num == BRT_WHITE) {
		snprintf(hex_out, max_len, "#FFFFFF");
		return;
	}

	GRCtoIBM(color_num, rgb_63, &ibm_c);
	r = (rgb_63[0] * 255) / 63;
	g = (rgb_63[1] * 255) / 63;
	b = (rgb_63[2] * 255) / 63;

	if (r < 0) r = 0; else if (r > 255) r = 255;
	if (g < 0) g = 0; else if (g > 255) g = 255;
	if (b < 0) b = 0; else if (b > 255) b = 255;

	snprintf(hex_out, max_len, "#%02X%02X%02X", r, g, b);
}

/********************************************************************/
/* Convert dash style index to SVG stroke-dasharray                 */
/********************************************************************/
static const char *get_svg_dasharray(int pattern)
{
	switch (pattern) {
		case SOLID:
		default:
			return NULL;
		case DOTTED:
			return "16,36";
		case DOT_DASH:
			return "90,36,16,36";
		case SHORT_DASH:
			return "45,45";
		case LONG_DASH:
			return "120,45";
		case LONG_DASH_THREE_DOTS:
			return "120,36,16,36,16,36,16,36";
		case LONG_DASH_SHORT_DASH:
			return "120,36,45,36";
		case MEDIUM_DASH:
			return "75,45";
		case FINE_DOT:
			return "8,24";
	}
}

/********************************************************************/
/* Write companion HTML viewer with interactive pan/zoom            */
/********************************************************************/
/********************************************************************/
/* Write initial companion HTML viewer before plotting starts       */
/********************************************************************/
static void write_initial_html_viewer(const char *svg_filename)
{
	char html_filename[512];
	char base_name[256];
	const char *leaf_name;
	char *dot;
	FILE *hfp;

	strncpy(base_name, svg_filename, sizeof(base_name) - 1);
	base_name[sizeof(base_name) - 1] = '\0';
	dot = strrchr(base_name, '.');
	if (dot != NULL)
		*dot = '\0';

	leaf_name = strrchr(base_name, '/');
	if (leaf_name != NULL)
		leaf_name++;
	else {
		leaf_name = strrchr(base_name, '\\');
		if (leaf_name != NULL)
			leaf_name++;
		else
			leaf_name = base_name;
	}

	snprintf(html_filename, sizeof(html_filename), "%s.html", base_name);
	ensure_parent_dirs(html_filename);
	hfp = fopen(html_filename, "w");
	if (hfp == NULL)
		return;

	fprintf(hfp, "<!DOCTYPE html>\n<html>\n<head>\n");
	fprintf(hfp, "  <meta charset=\"utf-8\">\n");
	fprintf(hfp, "  <title>GraphiC Plot Viewer - %s</title>\n", leaf_name);
	fprintf(hfp, "  <style>\n");
	fprintf(hfp, "    body { margin: 0; padding: 0; background: #1e1e1e; color: #eee; font-family: -apple-system, BlinkMacSystemFont, sans-serif; display: flex; flex-direction: column; height: 100vh; overflow: hidden; }\n");
	fprintf(hfp, "    header { background: #2d2d2d; padding: 10px 20px; display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid #444; }\n");
	fprintf(hfp, "    h1 { margin: 0; font-size: 16px; font-weight: 600; color: #61afef; display: flex; align-items: center; gap: 8px; }\n");
	fprintf(hfp, "    .badge { font-size: 11px; padding: 2px 8px; border-radius: 10px; background: #e5c07b; color: #1e1e1e; font-weight: bold; }\n");
	fprintf(hfp, "    .controls { display: flex; gap: 10px; }\n");
	fprintf(hfp, "    button { background: #3e4451; color: #fff; border: 1px solid #555; padding: 6px 14px; border-radius: 4px; cursor: pointer; font-size: 13px; transition: background 0.2s; }\n");
	fprintf(hfp, "    button:hover { background: #4b5263; }\n");
	fprintf(hfp, "    #container { flex: 1; position: relative; overflow: hidden; cursor: grab; background: #333; display: flex; justify-content: center; align-items: center; }\n");
	fprintf(hfp, "    #container:active { cursor: grabbing; }\n");
	fprintf(hfp, "    #viewport { width: 90%%; height: 90%%; box-shadow: 0 10px 30px rgba(0,0,0,0.5); background: #fff; border-radius: 4px; transition: transform 0.05s ease-out; display: flex; align-items: center; justify-content: center; }\n");
	fprintf(hfp, "    #viewport svg, #viewport img { width: 100%%; height: 100%%; display: block; }\n");
	fprintf(hfp, "  </style>\n");
	fprintf(hfp, "</head>\n<body>\n");
	fprintf(hfp, "  <header>\n");
	fprintf(hfp, "    <h1>GraphiC Plot: %s.svg <span class=\"badge\" id=\"status_badge\">Rendering...</span></h1>\n", leaf_name);
	fprintf(hfp, "    <div class=\"controls\">\n");
	fprintf(hfp, "      <button onclick=\"resetZoom()\">Reset View</button>\n");
	fprintf(hfp, "      <button onclick=\"zoom(1.25)\">Zoom In (+)</button>\n");
	fprintf(hfp, "      <button onclick=\"zoom(0.8)\">Zoom Out (-)</button>\n");
	fprintf(hfp, "      <button onclick=\"downloadSVG()\">Download SVG</button>\n");
	fprintf(hfp, "    </div>\n");
	fprintf(hfp, "  </header>\n");
	fprintf(hfp, "  <div id=\"container\">\n");
	fprintf(hfp, "    <div id=\"viewport\">\n");
	fprintf(hfp, "      <div id=\"loading_msg\" style=\"text-align: center; color: #888; font-family: monospace;\">\n");
	fprintf(hfp, "        <div style=\"font-size: 16px; color: #61afef; margin-bottom: 6px;\">GraphiC 2026 Live Plot</div>\n");
	fprintf(hfp, "        <div>Plotting started... Waiting for render completion</div>\n");
	fprintf(hfp, "      </div>\n");
	fprintf(hfp, "    </div>\n");
	fprintf(hfp, "  </div>\n");
	fprintf(hfp, "  <script>\n");
	fprintf(hfp, "    let scale = 1, panX = 0, panY = 0, isDragging = false, startX, startY;\n");
	fprintf(hfp, "    const vp = document.getElementById('viewport');\n");
	fprintf(hfp, "    const cont = document.getElementById('container');\n");
	fprintf(hfp, "    function update() { vp.style.transform = `translate(${panX}px, ${panY}px) scale(${scale})`; }\n");
	fprintf(hfp, "    function zoom(factor) { scale *= factor; update(); }\n");
	fprintf(hfp, "    function resetZoom() { scale = 1; panX = 0; panY = 0; update(); }\n");
	fprintf(hfp, "    cont.addEventListener('wheel', e => { e.preventDefault(); const factor = e.deltaY < 0 ? 1.1 : 0.9; zoom(factor); });\n");
	fprintf(hfp, "    cont.addEventListener('mousedown', e => { isDragging = true; startX = e.clientX - panX; startY = e.clientY - panY; });\n");
	fprintf(hfp, "    window.addEventListener('mousemove', e => { if (isDragging) { panX = e.clientX - startX; panY = e.clientY - startY; update(); } });\n");
	fprintf(hfp, "    window.addEventListener('mouseup', () => { isDragging = false; });\n");
	fprintf(hfp, "    function downloadSVG() {\n");
	fprintf(hfp, "      const svg = document.querySelector('#viewport svg');\n");
	fprintf(hfp, "      if (!svg) return;\n");
	fprintf(hfp, "      const blob = new Blob(['<?xml version=\"1.0\" encoding=\"UTF-8\"?>\\n' + svg.outerHTML], { type: 'image/svg+xml;charset=utf-8' });\n");
	fprintf(hfp, "      const url = URL.createObjectURL(blob);\n");
	fprintf(hfp, "      const a = document.createElement('a');\n");
	fprintf(hfp, "      a.href = url;\n");
	fprintf(hfp, "      a.download = '%s.svg';\n", leaf_name);
	fprintf(hfp, "      document.body.appendChild(a);\n");
	fprintf(hfp, "      a.click();\n");
	fprintf(hfp, "      document.body.removeChild(a);\n");
	fprintf(hfp, "      URL.revokeObjectURL(url);\n");
	fprintf(hfp, "    }\n");
	fprintf(hfp, "    let pollActive = true;\n");
	fprintf(hfp, "    function pollPlot() {\n");
	fprintf(hfp, "      if (!pollActive) return;\n");
	fprintf(hfp, "      let testImg = new Image();\n");
	fprintf(hfp, "      testImg.src = '%s.svg?t=' + Date.now();\n", leaf_name);
	fprintf(hfp, "      testImg.onload = function() {\n");
	fprintf(hfp, "        vp.innerHTML = '';\n");
	fprintf(hfp, "        testImg.style.width = '100%%';\n");
	fprintf(hfp, "        testImg.style.height = '100%%';\n");
	fprintf(hfp, "        testImg.style.display = 'block';\n");
	fprintf(hfp, "        vp.appendChild(testImg);\n");
	fprintf(hfp, "        const b = document.getElementById('status_badge');\n");
	fprintf(hfp, "        if (b) { b.textContent = 'Live Active'; b.style.background = '#98c379'; }\n");
	fprintf(hfp, "        pollActive = false;\n");
	fprintf(hfp, "      };\n");
	fprintf(hfp, "    }\n");
	fprintf(hfp, "    setInterval(pollPlot, 250);\n");
	fprintf(hfp, "  </script>\n");
	fprintf(hfp, "</body>\n</html>\n");

	fclose(hfp);
}

/********************************************************************/
/* Write companion HTML viewer with interactive pan/zoom            */
/********************************************************************/
static void write_html_viewer(const char *svg_filename)
{
	char html_filename[512];
	char base_name[256];
	const char *leaf_name;
	char *dot;
	FILE *hfp, *sfp;
	char line[2048];

	strncpy(base_name, svg_filename, sizeof(base_name) - 1);
	base_name[sizeof(base_name) - 1] = '\0';
	dot = strrchr(base_name, '.');
	if (dot != NULL)
		*dot = '\0';

	/* Find leaf filename (without directory path) for titles and downloads */
	leaf_name = strrchr(base_name, '/');
	if (leaf_name != NULL)
		leaf_name++;
	else {
		leaf_name = strrchr(base_name, '\\');
		if (leaf_name != NULL)
			leaf_name++;
		else
			leaf_name = base_name;
	}

	snprintf(html_filename, sizeof(html_filename), "%s.html", base_name);
	ensure_parent_dirs(html_filename);
	hfp = fopen(html_filename, "w");
	if (hfp == NULL)
		return;

	fprintf(hfp, "<!DOCTYPE html>\n<html>\n<head>\n");
	fprintf(hfp, "  <meta charset=\"utf-8\">\n");
	fprintf(hfp, "  <title>GraphiC Plot Viewer - %s</title>\n", leaf_name);
	fprintf(hfp, "  <style>\n");
	fprintf(hfp, "    body { margin: 0; padding: 0; background: #1e1e1e; color: #eee; font-family: -apple-system, BlinkMacSystemFont, sans-serif; display: flex; flex-direction: column; height: 100vh; overflow: hidden; }\n");
	fprintf(hfp, "    header { background: #2d2d2d; padding: 10px 20px; display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid #444; }\n");
	fprintf(hfp, "    h1 { margin: 0; font-size: 16px; font-weight: 600; color: #61afef; display: flex; align-items: center; gap: 8px; }\n");
	fprintf(hfp, "    .badge { font-size: 11px; padding: 2px 8px; border-radius: 10px; background: #98c379; color: #1e1e1e; font-weight: bold; }\n");
	fprintf(hfp, "    .controls { display: flex; gap: 10px; }\n");
	fprintf(hfp, "    button { background: #3e4451; color: #fff; border: 1px solid #555; padding: 6px 14px; border-radius: 4px; cursor: pointer; font-size: 13px; transition: background 0.2s; }\n");
	fprintf(hfp, "    button:hover { background: #4b5263; }\n");
	fprintf(hfp, "    #container { flex: 1; position: relative; overflow: hidden; cursor: grab; background: #333; display: flex; justify-content: center; align-items: center; }\n");
	fprintf(hfp, "    #container:active { cursor: grabbing; }\n");
	fprintf(hfp, "    #viewport { width: 90%%; height: 90%%; box-shadow: 0 10px 30px rgba(0,0,0,0.5); background: #fff; border-radius: 4px; transition: transform 0.05s ease-out; display: flex; align-items: center; justify-content: center; }\n");
	fprintf(hfp, "    #viewport svg { width: 100%%; height: 100%%; display: block; }\n");
	fprintf(hfp, "  </style>\n");
	fprintf(hfp, "</head>\n<body>\n");
	fprintf(hfp, "  <header>\n");
	fprintf(hfp, "    <h1>GraphiC Plot: %s.svg <span class=\"badge\">Rendered</span></h1>\n", leaf_name);
	fprintf(hfp, "    <div class=\"controls\">\n");
	fprintf(hfp, "      <button onclick=\"resetZoom()\">Reset View</button>\n");
	fprintf(hfp, "      <button onclick=\"zoom(1.25)\">Zoom In (+)</button>\n");
	fprintf(hfp, "      <button onclick=\"zoom(0.8)\">Zoom Out (-)</button>\n");
	fprintf(hfp, "      <button onclick=\"downloadSVG()\">Download SVG</button>\n");
	fprintf(hfp, "    </div>\n");
	fprintf(hfp, "  </header>\n");
	fprintf(hfp, "  <div id=\"container\">\n");
	fprintf(hfp, "    <div id=\"viewport\">\n");

	/* Inline SVG content directly so browser never blocks on local file/CORS policies */
	sfp = fopen(svg_filename, "r");
	if (sfp != NULL) {
		while (fgets(line, sizeof(line), sfp) != NULL) {
			if (strncmp(line, "<?xml", 5) == 0)
				continue;
			fputs(line, hfp);
		}
		fclose(sfp);
	}

	fprintf(hfp, "    </div>\n");
	fprintf(hfp, "  </div>\n");
	fprintf(hfp, "  <script>\n");
	fprintf(hfp, "    let scale = 1, panX = 0, panY = 0, isDragging = false, startX, startY;\n");
	fprintf(hfp, "    const vp = document.getElementById('viewport');\n");
	fprintf(hfp, "    const cont = document.getElementById('container');\n");
	fprintf(hfp, "    function update() { vp.style.transform = `translate(${panX}px, ${panY}px) scale(${scale})`; }\n");
	fprintf(hfp, "    function zoom(factor) { scale *= factor; update(); }\n");
	fprintf(hfp, "    function resetZoom() { scale = 1; panX = 0; panY = 0; update(); }\n");
	fprintf(hfp, "    cont.addEventListener('wheel', e => { e.preventDefault(); const factor = e.deltaY < 0 ? 1.1 : 0.9; zoom(factor); });\n");
	fprintf(hfp, "    cont.addEventListener('mousedown', e => { isDragging = true; startX = e.clientX - panX; startY = e.clientY - panY; });\n");
	fprintf(hfp, "    window.addEventListener('mousemove', e => { if (isDragging) { panX = e.clientX - startX; panY = e.clientY - startY; update(); } });\n");
	fprintf(hfp, "    window.addEventListener('mouseup', () => { isDragging = false; });\n");
	fprintf(hfp, "    function downloadSVG() {\n");
	fprintf(hfp, "      const svg = document.querySelector('#viewport svg');\n");
	fprintf(hfp, "      if (!svg) return;\n");
	fprintf(hfp, "      const blob = new Blob(['<?xml version=\"1.0\" encoding=\"UTF-8\"?>\\n' + svg.outerHTML], { type: 'image/svg+xml;charset=utf-8' });\n");
	fprintf(hfp, "      const url = URL.createObjectURL(blob);\n");
	fprintf(hfp, "      const a = document.createElement('a');\n");
	fprintf(hfp, "      a.href = url;\n");
	fprintf(hfp, "      a.download = '%s.svg';\n", leaf_name);
	fprintf(hfp, "      document.body.appendChild(a);\n");
	fprintf(hfp, "      a.click();\n");
	fprintf(hfp, "      document.body.removeChild(a);\n");
	fprintf(hfp, "      URL.revokeObjectURL(url);\n");
	fprintf(hfp, "    }\n");
	fprintf(hfp, "  </script>\n");
	fprintf(hfp, "</body>\n</html>\n");

	fclose(hfp);
}

static void write_svg_gradient_defs(FILE *fp);

/********************************************************************/
/* Emit SVG vector pattern definitions (patterns 1..16)            */
/********************************************************************/
static void write_svg_pattern_defs(FILE *fp)
{
	if (!fp) return;

	/*/* Pattern 1: Horizontal lines (fine) */
	fprintf(fp, "    <pattern id=\"gpc_pat_1\" width=\"40\" height=\"40\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"40\" height=\"40\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"20\" x2=\"40\" y2=\"20\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 2: Vertical lines (fine) */
	fprintf(fp, "    <pattern id=\"gpc_pat_2\" width=\"40\" height=\"40\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"40\" height=\"40\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <line x1=\"20\" y1=\"0\" x2=\"20\" y2=\"40\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 3: Diagonal 45° (fine, /) */
	fprintf(fp, "    <pattern id=\"gpc_pat_3\" width=\"40\" height=\"40\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"40\" height=\"40\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"40\" x2=\"40\" y2=\"0\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "      <line x1=\"-20\" y1=\"20\" x2=\"20\" y2=\"-20\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "      <line x1=\"20\" y1=\"60\" x2=\"60\" y2=\"20\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 4: Diagonal 135° (fine, \\) */
	fprintf(fp, "    <pattern id=\"gpc_pat_4\" width=\"40\" height=\"40\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"40\" height=\"40\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"0\" x2=\"40\" y2=\"40\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "      <line x1=\"-20\" y1=\"20\" x2=\"20\" y2=\"60\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "      <line x1=\"20\" y1=\"-20\" x2=\"60\" y2=\"20\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 5: Crosshatch orthogonal (fine, +) */
	fprintf(fp, "    <pattern id=\"gpc_pat_5\" width=\"40\" height=\"40\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"40\" height=\"40\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"20\" x2=\"40\" y2=\"20\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "      <line x1=\"20\" y1=\"0\" x2=\"20\" y2=\"40\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 6: Crosshatch diagonal (fine, X) */
	fprintf(fp, "    <pattern id=\"gpc_pat_6\" width=\"40\" height=\"40\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"40\" height=\"40\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"40\" x2=\"40\" y2=\"0\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"0\" x2=\"40\" y2=\"40\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 7: Horizontal lines (coarse) */
	fprintf(fp, "    <pattern id=\"gpc_pat_7\" width=\"80\" height=\"80\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"80\" height=\"80\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"40\" x2=\"80\" y2=\"40\" stroke=\"#000000\" stroke-width=\"8\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 8: Vertical lines (coarse) */
	fprintf(fp, "    <pattern id=\"gpc_pat_8\" width=\"80\" height=\"80\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"80\" height=\"80\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <line x1=\"40\" y1=\"0\" x2=\"40\" y2=\"80\" stroke=\"#000000\" stroke-width=\"8\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 9: Diagonal 45° (coarse, /) */
	fprintf(fp, "    <pattern id=\"gpc_pat_9\" width=\"80\" height=\"80\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"80\" height=\"80\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"80\" x2=\"80\" y2=\"0\" stroke=\"#000000\" stroke-width=\"8\"/>\n");
	fprintf(fp, "      <line x1=\"-40\" y1=\"40\" x2=\"40\" y2=\"-40\" stroke=\"#000000\" stroke-width=\"8\"/>\n");
	fprintf(fp, "      <line x1=\"40\" y1=\"120\" x2=\"120\" y2=\"40\" stroke=\"#000000\" stroke-width=\"8\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 10: Diagonal 135° (coarse, \\) */
	fprintf(fp, "    <pattern id=\"gpc_pat_10\" width=\"80\" height=\"80\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"80\" height=\"80\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"0\" x2=\"80\" y2=\"80\" stroke=\"#000000\" stroke-width=\"8\"/>\n");
	fprintf(fp, "      <line x1=\"-40\" y1=\"40\" x2=\"40\" y2=\"120\" stroke=\"#000000\" stroke-width=\"8\"/>\n");
	fprintf(fp, "      <line x1=\"40\" y1=\"-40\" x2=\"120\" y2=\"40\" stroke=\"#000000\" stroke-width=\"8\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 11: Crosshatch orthogonal (coarse, +) */
	fprintf(fp, "    <pattern id=\"gpc_pat_11\" width=\"80\" height=\"80\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"80\" height=\"80\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"40\" x2=\"80\" y2=\"40\" stroke=\"#000000\" stroke-width=\"8\"/>\n");
	fprintf(fp, "      <line x1=\"40\" y1=\"0\" x2=\"40\" y2=\"80\" stroke=\"#000000\" stroke-width=\"8\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 12: Crosshatch diagonal (coarse, X) */
	fprintf(fp, "    <pattern id=\"gpc_pat_12\" width=\"80\" height=\"80\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"80\" height=\"80\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"80\" x2=\"80\" y2=\"0\" stroke=\"#000000\" stroke-width=\"8\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"0\" x2=\"80\" y2=\"80\" stroke=\"#000000\" stroke-width=\"8\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 13: Dot Stipple / Grid (fine) */
	fprintf(fp, "    <pattern id=\"gpc_pat_13\" width=\"40\" height=\"40\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"40\" height=\"40\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <circle cx=\"20\" cy=\"20\" r=\"6\" fill=\"#000000\"/>\n");
	fprintf(fp, "      <circle cx=\"0\" cy=\"0\" r=\"6\" fill=\"#000000\"/>\n");
	fprintf(fp, "      <circle cx=\"40\" cy=\"0\" r=\"6\" fill=\"#000000\"/>\n");
	fprintf(fp, "      <circle cx=\"0\" cy=\"40\" r=\"6\" fill=\"#000000\"/>\n");
	fprintf(fp, "      <circle cx=\"40\" cy=\"40\" r=\"6\" fill=\"#000000\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 14: Dense Stipple / Staggered Dots */
	fprintf(fp, "    <pattern id=\"gpc_pat_14\" width=\"32\" height=\"32\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"32\" height=\"32\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <circle cx=\"8\" cy=\"8\" r=\"5\" fill=\"#000000\"/>\n");
	fprintf(fp, "      <circle cx=\"24\" cy=\"24\" r=\"5\" fill=\"#000000\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 15: Brick Pattern */
	fprintf(fp, "    <pattern id=\"gpc_pat_15\" width=\"64\" height=\"32\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"64\" height=\"32\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"16\" x2=\"64\" y2=\"16\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"32\" x2=\"64\" y2=\"32\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "      <line x1=\"32\" y1=\"0\" x2=\"32\" y2=\"16\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "      <line x1=\"0\" y1=\"16\" x2=\"0\" y2=\"32\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "      <line x1=\"64\" y1=\"16\" x2=\"64\" y2=\"32\" stroke=\"#000000\" stroke-width=\"6\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Pattern 16: Checkerboard */
	fprintf(fp, "    <pattern id=\"gpc_pat_16\" width=\"48\" height=\"48\" patternUnits=\"userSpaceOnUse\">\n");
	fprintf(fp, "      <rect width=\"48\" height=\"48\" fill=\"#ffffff\"/>\n");
	fprintf(fp, "      <rect x=\"0\" y=\"0\" width=\"24\" height=\"24\" fill=\"#000000\"/>\n");
	fprintf(fp, "      <rect x=\"24\" y=\"24\" width=\"24\" height=\"24\" fill=\"#000000\"/>\n");
	fprintf(fp, "    </pattern>\n");

	/* Scientific colormap linear gradients */
	write_svg_gradient_defs(fp);
}

extern int gpc_colormap_eval(int colormap_id, float t);

/********************************************************************/
/* Emit SVG linear gradient definitions for scientific colormaps    */
/********************************************************************/
static void write_svg_gradient_defs(FILE *fp)
{
	int cmap, step;
	if (!fp) return;

	for (cmap = 0; cmap < 7; cmap++) {
		/* Vertical gradient: min at bottom (y1=100%), max at top (y2=0%) */
		fprintf(fp, "    <linearGradient id=\"gpc_cmap_v_%d\" x1=\"0%%\" y1=\"100%%\" x2=\"0%%\" y2=\"0%%\">\n", cmap);
		for (step = 0; step <= 32; step++) {
			float t = (float)step / 32.0f;
			int clr = gpc_colormap_eval(cmap, t);
			int r = (clr >> 16) & 0xFF;
			int g = (clr >> 8) & 0xFF;
			int b = clr & 0xFF;
			fprintf(fp, "      <stop offset=\"%.2f%%\" stop-color=\"#%02X%02X%02X\"/>\n", t * 100.0f, r, g, b);
		}
		fprintf(fp, "    </linearGradient>\n");

		/* Horizontal gradient: min at left (x1=0%), max at right (x2=100%) */
		fprintf(fp, "    <linearGradient id=\"gpc_cmap_h_%d\" x1=\"0%%\" y1=\"0%%\" x2=\"100%%\" y2=\"0%%\">\n", cmap);
		for (step = 0; step <= 32; step++) {
			float t = (float)step / 32.0f;
			int clr = gpc_colormap_eval(cmap, t);
			int r = (clr >> 16) & 0xFF;
			int g = (clr >> 8) & 0xFF;
			int b = clr & 0xFF;
			fprintf(fp, "      <stop offset=\"%.2f%%\" stop-color=\"#%02X%02X%02X\"/>\n", t * 100.0f, r, g, b);
		}
		fprintf(fp, "    </linearGradient>\n");
	}
}

static char *svg_panel_d = NULL;
static size_t svg_panel_d_len = 0;
static size_t svg_panel_d_cap = 0;
static int svg_panel_pattern = 0;
static int svg_panel_border = 0;
static int svg_panel_b_color = 0;
static int svg_panel_pieces = 0;

static void flush_svg_panel(void)
{
	char fill_hex[64];
	char border_hex[16];

	if (svg_tmp_fp == NULL || svg_panel_pieces == 0 || svg_panel_d == NULL || svg_panel_d_len == 0) {
		svg_panel_pieces = 0;
		svg_panel_d_len = 0;
		return;
	}

	/* Determine fill style */
	if (svg_panel_pattern >= 1000 && svg_panel_pattern < 2000) {
		snprintf(fill_hex, sizeof(fill_hex), "url(#gpc_cmap_v_%d)", svg_panel_pattern - 1000);
	}
	else if (svg_panel_pattern >= 2000 && svg_panel_pattern < 3000) {
		snprintf(fill_hex, sizeof(fill_hex), "url(#gpc_cmap_h_%d)", svg_panel_pattern - 2000);
	}
	else if (svg_panel_pattern >= 1 && svg_panel_pattern <= 16) {
		snprintf(fill_hex, sizeof(fill_hex), "url(#gpc_pat_%d)", svg_panel_pattern);
	}
	else if (svg_panel_pattern < 0) {
		color_to_svg_hex(-svg_panel_pattern, fill_hex, sizeof(fill_hex));
	}
	else {
		color_to_svg_hex(svg_panel_pattern, fill_hex, sizeof(fill_hex));
	}

	/* Determine border stroke */
	if (svg_panel_border) {
		color_to_svg_hex(svg_panel_b_color, border_hex, sizeof(border_hex));
	} else {
		strcpy(border_hex, "none");
	}

	fprintf(svg_tmp_fp, "  <path d=\"%s\" fill=\"%s\" fill-rule=\"evenodd\" stroke=\"%s\" stroke-width=\"%s\"/>\n",
		svg_panel_d, fill_hex, border_hex, svg_panel_border ? "1" : "0");

	svg_panel_pieces = 0;
	svg_panel_d_len = 0;
	if (svg_panel_d) svg_panel_d[0] = '\0';
}

/********************************************************************/
/* Finalize SVG document, calculate cropped viewBox, output file    */
/********************************************************************/
static void finalize_svg_plot(void)
{
	flush_svg_panel();
	if (svg_panel_d != NULL) {
		free(svg_panel_d);
		svg_panel_d = NULL;
		svg_panel_d_cap = 0;
		svg_panel_d_len = 0;
	}

	if (svg_tmp_fp != NULL) {
		int vx = 0, vy = 0, vw = canvas_w, vh = canvas_h;

		if (svg_autocrop_enabled && svg_has_drawn) {
			/* Convert margin from inches to device units: 8192 / 9.0" = 910.2222 units/inch */
			int margin_units = (int)(svg_crop_margin * 910.2222f + 0.5f);
			if (margin_units < 0) margin_units = 0;

			vx = svg_bbox_min_x - margin_units;
			vy = svg_bbox_min_y - margin_units;
			vw = (svg_bbox_max_x - svg_bbox_min_x) + 2 * margin_units;
			vh = (svg_bbox_max_y - svg_bbox_min_y) + 2 * margin_units;

			if (vw <= 0) vw = 100;
			if (vh <= 0) vh = 100;
		}

		ensure_parent_dirs(current_svg_path);
		svg_fp = fopen(current_svg_path, "w");
		if (svg_fp != NULL) {
			char buf[8192];
			size_t n;

			/* Write SVG document header with calculated or full viewBox */
			fprintf(svg_fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
			fprintf(svg_fp, "<svg xmlns=\"http://www.w3.org/2000/svg\"\n");
			fprintf(svg_fp, "     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n");
			fprintf(svg_fp, "     viewBox=\"%d %d %d %d\"\n", vx, vy, vw, vh);
			fprintf(svg_fp, "     width=\"100%%\" height=\"100%%\">\n");
			fprintf(svg_fp, "  <defs>\n");
			fprintf(svg_fp, "    <style type=\"text/css\">\n");
			fprintf(svg_fp, "      line, polyline, path, polygon { stroke-linejoin: round; stroke-linecap: round; }\n");
			fprintf(svg_fp, "    </style>\n");
			write_svg_pattern_defs(svg_fp);
			fprintf(svg_fp, "  </defs>\n");

			/* Background rect matching the cropped (or full) viewBox */
			if (svg_has_bg && svg_bg_color_hex[0] != '\0') {
				fprintf(svg_fp, "  <!-- Background -->\n");
				fprintf(svg_fp, "  <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"%s\" id=\"bg\"/>\n",
					vx, vy, vw, vh, svg_bg_color_hex);
			}

			/* Copy drawing commands from temporary stream */
			rewind(svg_tmp_fp);
			while ((n = fread(buf, 1, sizeof(buf), svg_tmp_fp)) > 0) {
				fwrite(buf, 1, n, svg_fp);
			}
			fclose(svg_tmp_fp);
			svg_tmp_fp = NULL;

			fprintf(svg_fp, "</svg>\n");
			fclose(svg_fp);
			svg_fp = NULL;

			/* Write companion interactive HTML viewer */
			write_html_viewer(current_svg_path);
			printf("[GraphiC] Rendered SVG plot to: %s\n", current_svg_path);
		} else {
			fclose(svg_tmp_fp);
			svg_tmp_fp = NULL;
		}
	}

	svg_in_plot = 0;
	svg_has_drawn = 0;
	svg_has_bg = 0;
	svg_bg_color_hex[0] = '\0';
}

/********************************************************************/
/* Main driver function table dispatcher                            */
/********************************************************************/
int TYPE f_tableq(void G_FAR *fstruc)
{
	int *lpFunc = (int *)fstruc;
	int func_code;
	char hex_color[16];

	if (fstruc == NULL)
		return 0;

	func_code = *lpFunc;

	switch (func_code) {
		case INITIALIZE: {
			struct GINITQ *gc = (struct GINITQ *)fstruc;
			const char *base_name;
			char name_buf[512];
			char *dot;

			if (svg_in_plot || svg_tmp_fp != NULL) {
				finalize_svg_plot();
			}

			gc->vector = ALL_POLYGONS | VECTOR_DEVICE;
			gc->vmax = MAXRESYMO;
			gc->hmax = MAXRESXMO;
			gc->max_psides = 0; /* No limit */

			if (tkfnameq != NULL && strlen(tkfnameq) > 0) {
				strncpy(name_buf, tkfnameq, sizeof(name_buf) - 1);
				name_buf[sizeof(name_buf) - 1] = '\0';
				dot = strstr(name_buf, ".tkf");
				if (dot != NULL) *dot = '\0';
				dot = strstr(name_buf, ".TKF");
				if (dot != NULL) *dot = '\0';
				dot = strstr(name_buf, ".svg");
				if (dot != NULL) *dot = '\0';
				dot = strstr(name_buf, ".SVG");
				if (dot != NULL) *dot = '\0';
				base_name = name_buf;
			} else {
				base_name = "graphic_plot";
			}

			if (svg_page_num > 0) {
				snprintf(current_svg_path, sizeof(current_svg_path), "%s_%d.svg", base_name, svg_page_num);
				snprintf(current_html_path, sizeof(current_html_path), "%s_%d.html", base_name, svg_page_num);
			} else {
				snprintf(current_svg_path, sizeof(current_svg_path), "%s.svg", base_name);
				snprintf(current_html_path, sizeof(current_html_path), "%s.html", base_name);
			}

			svg_tmp_fp = gpc_tmpfile();
			if (svg_tmp_fp == NULL) {
				fprintf(stderr, "[GraphiC SVG Driver] Could not create temporary buffer for: %s\n", current_svg_path);
				return -1;
			}

			svg_has_drawn = 0;
			svg_has_bg = 0;
			svg_bg_color_hex[0] = '\0';
			svg_in_plot = 1;
			svg_page_num++;

			/* Write initial companion HTML viewer immediately so user can preview right at plot start */
			write_initial_html_viewer(current_svg_path);
			break;
		}

		case CLEAR: {
			struct GCLEARQ *gc = (struct GCLEARQ *)fstruc;
			flush_svg_panel();
			svg_has_bg = 1;
			color_to_svg_hex(gc->color, svg_bg_color_hex, sizeof(svg_bg_color_hex));
			break;
		}

		case G_LINE: {
			struct GLINEQ *gl = (struct GLINEQ *)fstruc;
			const char *dash;
			int stroke_width;
			int half_stroke;

			if (svg_tmp_fp == NULL)
				break;

			flush_svg_panel();

			color_to_svg_hex(gl->color, hex_color, sizeof(hex_color));
			dash = get_svg_dasharray(gl->pattern);

			if (gl->font_char) {
				stroke_width = (gl->thick <= 0) ? 8 : (8 + gl->thick * 2);
			} else {
				stroke_width = (gl->thick <= 0) ? 8 : (gl->thick <= 8 ? (8 + gl->thick * 2) : gl->thick);
			}
			half_stroke = (stroke_width + 1) / 2;

			/* Check if drawing a polyline with multiple points */
			if (gl->action != 0 && gl->xyp != NULL && gl->xyp[0].x > 0) {
				if (gl->font_char && gl->npts > 1) {
					/* Font character glyph consisting of one or more sub-strokes */
					int idx = 0;
					while (idx < gl->npts && gl->xyp[idx].x > 0) {
						int count = gl->xyp[idx].x;
						int j;
						if (idx + count >= gl->npts) {
							count = gl->npts - 1 - idx;
							if (count <= 0) break;
						}
						if (count == 1) {
							int sx = gl->xyp[idx + 1].x;
							int sy = max_y_coord - gl->xyp[idx + 1].y;
							svg_update_bbox_pt(sx, sy, half_stroke);
							fprintf(svg_tmp_fp, "  <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"%s\" stroke-width=\"%d\" stroke-linecap=\"round\"/>\n",
								sx, sy, sx, sy, hex_color, stroke_width);
						} else if (count > 1) {
							fprintf(svg_tmp_fp, "  <polyline points=\"");
							for (j = 1; j <= count; j++) {
								int sx = gl->xyp[idx + j].x;
								int sy = max_y_coord - gl->xyp[idx + j].y;
								svg_update_bbox_pt(sx, sy, half_stroke);
								fprintf(svg_tmp_fp, "%d,%d%s", sx, sy, (j < count) ? " " : "");
							}
							fprintf(svg_tmp_fp, "\" fill=\"none\" stroke=\"%s\" stroke-width=\"%d\"",
								hex_color, stroke_width);
							if (dash != NULL)
								fprintf(svg_tmp_fp, " stroke-dasharray=\"%s\"", dash);
							fprintf(svg_tmp_fp, " stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n");
						}
						idx += count + 1;
					}
				} else {
					/* Regular non-font polyline */
					int n = gl->xyp[0].x;
					int j;
					if (n == 1) {
						int sx = gl->xyp[1].x;
						int sy = max_y_coord - gl->xyp[1].y;
						svg_update_bbox_pt(sx, sy, half_stroke);
						fprintf(svg_tmp_fp, "  <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"%s\" stroke-width=\"%d\" stroke-linecap=\"round\"/>\n",
							sx, sy, sx, sy, hex_color, stroke_width);
					} else if (n > 1) {
						fprintf(svg_tmp_fp, "  <polyline points=\"");
						for (j = 1; j <= n; j++) {
							int sx = gl->xyp[j].x;
							int sy = max_y_coord - gl->xyp[j].y;
							svg_update_bbox_pt(sx, sy, half_stroke);
							fprintf(svg_tmp_fp, "%d,%d%s", sx, sy, (j < n) ? " " : "");
						}
						fprintf(svg_tmp_fp, "\" fill=\"none\" stroke=\"%s\" stroke-width=\"%d\"",
							hex_color, stroke_width);
						if (dash != NULL)
							fprintf(svg_tmp_fp, " stroke-dasharray=\"%s\"", dash);
						fprintf(svg_tmp_fp, " stroke-linecap=\"round\" stroke-linejoin=\"round\"/>\n");
					}
				}
			}
			else {
				/* Single line segment */
				int x1 = gl->pfx;
				int y1 = max_y_coord - gl->pfy;
				int x2 = gl->ptx;
				int y2 = max_y_coord - gl->pty;

				svg_update_bbox_pt(x1, y1, half_stroke);
				svg_update_bbox_pt(x2, y2, half_stroke);

				fprintf(svg_tmp_fp, "  <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"%s\" stroke-width=\"%d\"",
					x1, y1, x2, y2, hex_color, stroke_width);
				if (dash != NULL)
					fprintf(svg_tmp_fp, " stroke-dasharray=\"%s\"", dash);
				fprintf(svg_tmp_fp, " stroke-linecap=\"round\"/>\n");
			}
			break;
		}

		case PANL: {
			struct GPANELQ *gp = (struct GPANELQ *)fstruc;
			int j;
			char chunk[64];
			int chunk_len;

			if (svg_tmp_fp == NULL || gp->nsides < 3 || gp->xyp == NULL)
				break;

			if (svg_panel_pieces == 0) {
				svg_panel_pattern = gp->pattern;
				svg_panel_border = gp->border;
				svg_panel_b_color = gp->b_color;
			} else if (gp->pattern != svg_panel_pattern || gp->border != svg_panel_border || gp->b_color != svg_panel_b_color) {
				flush_svg_panel();
				svg_panel_pattern = gp->pattern;
				svg_panel_border = gp->border;
				svg_panel_b_color = gp->b_color;
			}

			/* Append sub-path for this piece: "M sx,sy L sx,sy ... Z " */
			for (j = 0; j < gp->nsides; j++) {
				int sx = gp->xyp[j].x;
				int sy = max_y_coord - gp->xyp[j].y;
				svg_update_bbox_pt(sx, sy, 0);

				if (j == 0)
					chunk_len = snprintf(chunk, sizeof(chunk), "M %d,%d ", sx, sy);
				else
					chunk_len = snprintf(chunk, sizeof(chunk), "L %d,%d ", sx, sy);

				if (svg_panel_d_len + chunk_len + 8 >= svg_panel_d_cap) {
					size_t new_cap = (svg_panel_d_cap == 0) ? 4096 : svg_panel_d_cap * 2;
					while (svg_panel_d_len + chunk_len + 8 >= new_cap) new_cap *= 2;
					svg_panel_d = (char *)realloc(svg_panel_d, new_cap);
					svg_panel_d_cap = new_cap;
				}
				memcpy(svg_panel_d + svg_panel_d_len, chunk, chunk_len);
				svg_panel_d_len += chunk_len;
				svg_panel_d[svg_panel_d_len] = '\0';
			}
			if (svg_panel_d_len + 4 >= svg_panel_d_cap) {
				svg_panel_d_cap = svg_panel_d_cap * 2 + 64;
				svg_panel_d = (char *)realloc(svg_panel_d, svg_panel_d_cap);
			}
			svg_panel_d[svg_panel_d_len++] = 'Z';
			svg_panel_d[svg_panel_d_len++] = ' ';
			svg_panel_d[svg_panel_d_len] = '\0';

			svg_panel_pieces++;
			break;
		}

		case PANPROC:
			flush_svg_panel();
			break;

		case SIZE_PAGE: {
			struct G_PAGE_SIZE *ps = (struct G_PAGE_SIZE *)fstruc;
			(void)ps;
			break;
		}

		case QUIT: {
			finalize_svg_plot();
			svg_page_num = 0;
			break;
		}

		case CURSOR:
		case REDRAW:
		case SCRNTYPE:
		default:
			break;
	}

	return 0;
}

const char *gpc_get_svg_path(void)
{
	return current_svg_path;
}

const char *gpc_get_html_path(void)
{
	return current_html_path;
}

int gpc_is_svg_active(void)
{
	return svg_in_plot;
}

void gpc_flush_svg(void)
{
	if (svg_tmp_fp != NULL) {
		finalize_svg_plot();
	}
}
