/*==============================================================================
 * GraphiC Classic C Suite: Multi_y
 *
 * How to Compile & Run (from GraphiC repository root):
 *   make examples
 *   ./bin/Multi_y
 *
 * Direct Command-Line Compilation:
 *   clang -O2 -IInclude -DGPC_PORTABLE=1 c/examples/Multi_y.c lib/libgraphic.a -lm -o bin/Multi_y
 *   ./bin/Multi_y
 *============================================================================*/

/**********************************************************************
This program illustrates one method for creating a plot with multiple Y
axes. In this example we make use of contexts as if we were going to plot
simultaneous curves. Each coordinate system is established and associated
with a context which allows us to change between the various axes at will.

The basic steps are outlined below.
    1.  Call physor(x, y) to set the location of the origin.
    2.  Draw the axes.
        1.  Drawing the X axis and the adjacent Y axis
        2.  Turn of the X axis drawing.
        3.  Draw the additional Y axes. Each Y axis is shifted using
            the orel() function.
        4.  Reset the origin by turning all axes drawing off and repeating
            the call to physor() and the first call to the axes drawing
            routine. In this example the function DrawAxes() performs these
            steps.
    3.  Establish a transform for each Y axis.
        1.  Call context() to associate the coordinate system with a context
        2.  Call the page defining routines to initialize variables for
            the active context
        3.  Call the axes drawing routine to establish the scaling. Use
            the same X limits for each Y axis.
    4.  You can now use context(n) to switch between the axes.

This example draws 4 Y axes with a common X axis. In the interest of
simplicity no data is generated. The symbol and string plotting functions
take the place of data generating code.
**********************************************************************/

#define NPLOTS 4

#include <graphic.h>

void DrawAxes(void);
void SetTransforms(void);
void DefinePage(void);

void GPC_MAIN(void)
{
    bgnplot(1, 'g', "multi_y.tkf");        /* Initialize the plot. */
    startplot(BRT_WHITE);             /* Set the background color. */
    font(1, "swiss.fnt", '\310');          /* Load the SWISS font. */
    simuplot(NPLOTS);             /* Reserve space for 4 contexts. */
    DefinePage();                 /* Call the page setup routines. */

    plotabsolute(1);                /* Do not override plot sizes. */
    DrawAxes();                                  /* Draw the axes. */
    SetTransforms();                 /* Establish context scaling. */

    /* CONTEXTS MAY BE CHGANGED AT WILL. */
    context(1);
    fontfill(2, -GREEN, GREEN);
    pltfnt(1.15f, 1.f, "\310Second context at x = 1 and y = 1", .15f, 0);

    context(3);
    fontfill(2, -BROWN, BROWN);
    pltfnt(1.15f, 1.f, "\310Forth context at x = 1 and y = 1", .15f, 0);

    context(0);
    fontfill(2, -BLUE, BLUE);
    pltfnt(1.15f, 1.f, "\310First context at x = 1 and y = 1", .15f, 0);

    context(2);
    fontfill(2, -RED, RED);
    pltfnt(1.15f, 1.f, "\310Third context at x = 1 and y = 1", .15f, 0);

    context(0);
    color(BLUE);
    symbol(1.f, 1.f, 11);

    context(2);
    color(RED);
    symbol(1.f, 1.f, 13);

    context(1);
    color(GREEN);
    symbol(1.f, 1.f, 12);

    context(3);
    color(BROWN);
    symbol(1.f, 1.f, 14);

    simuplot(0);                        /* Free up context memory. */
    endplot();                           /* Wait for instructions. */
    stopplot();              /* Close files and free up resources. */
}

/********************************************************************
Draw the axes. The first drawing command draws an X and a Y axes.
For the other three axes, the X axis is turned of, the origin is
shifted and the remaining Y axes are drawn. After the drawing is
complete the origin is reset so that the transform is correct for
context 0.
********************************************************************/

void DrawAxes(void)
{
    color(BLUE);
    fillfont(1);
    graf("%-4.2f", 0.f, 1.f, 10.f, "%-4.2f", 0.f, 2.f, 12.f);
    axesoff(GPC_YAXIS | GPC_YLAB);
    orel(-.35f, 0);
    color(GREEN);
    graf("%-4.2f", 0.f, 1.f, 10.f, "%-4.2f", 0.f, 1.f, 6.f);
    orel(-.35f, 0);
    color(RED);
    graf("%-4.2f", 0.f, 1.f, 10.f, "%-4.2f", 0.f, .5f, 3.f);
    orel(-.35f, 0);
    color(BROWN);
    graf("%-4.2f", 0.f, 1.f, 10.f, "%-4.2f", 0.f, .25f, 1.5f);
    axesoff(GPC_AXESOFF);
    physor(1.5f, .4f);
    graf("%-4.2f", 0.f, 1.f, 10.f, "%-4.2f", 0.f, 2.f, 12.f);
}

/********************************************************************
In GraphiC, the data transforms are established by the axes drawing
routines. To set up the transforms for our multiple Y axes, we turn
off the axes drawing and call the axes routine for each cooridinate
system needed.
********************************************************************/

void SetTransforms(void)
{
    graf("%-4.2f", 0.f, 1.f, 10.f, "%-4.2f", 0.f, 2.f, 12.f);
    context(1);
    DefinePage();
    graf("%-4.2f", 0.f, 1.f, 10.f, "%-4.2f", 0.f, 1.f, 6.f);
    context(2);
    DefinePage();
    graf("%-4.2f", 0.f, 1.f, 10.f, "%-4.2f", 0.f, .5f, 3.f);
    context(3);
    DefinePage();
    graf("%-4.2f", 0.f, 1.f, 10.f, "%-4.2f", 0.f, .25f, 1.5f);
}

/********************************************************************
Call the page and plot diminsioning routines. We put these in a
function call because in this example each context uses the same
arguments. If this example was illustrating simultaneous plotting,
these calls would need to be separate.
********************************************************************/

void DefinePage(void)
{
    page(9.0f, 6.884f);
    area2d(7.25f, 5.5f);
    physor(1.5f, .4f);
}
