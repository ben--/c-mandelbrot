#include <gd.h>

#include <complex.h>
#include <stdbool.h>
#include <stdio.h>

#include "mandelbrot.h"

const size_t MAXIMUM_ITERATIONS = 100;
int escape_time_colors[5];

static void
color_all_pixels(gdImagePtr im, size_t width, size_t height, int color)
{
    gdImageFilledRectangle(im, 0, 0, width - 1, height - 1, color);
}

static double
horizontal_pixel_to_x_value(extremes_t extremes, size_t width, int horizontal_pixel)
{
    const double minimum_x = creal(extremes.lower_left);
    const double maximum_x = creal(extremes.upper_right);

    return minimum_x
        + horizontal_pixel / (width / (maximum_x - minimum_x));
}

static double
vertical_pixel_to_y_value(extremes_t extremes, size_t height, int vertical_pixel)
{
    const double minimum_y = cimag(extremes.lower_left);
    const double maximum_y = cimag(extremes.upper_right);

    return minimum_y
        + vertical_pixel / (height / (maximum_y - minimum_y));
}

extremes_t
get_extreme_coordinates(size_t width, size_t height, double range)
{
    extremes_t coords;

    if (width < height) {
        coords.lower_left = 0.0 - (range * width/height) / 2.0
            - I * (range) / 2.0;
        coords.upper_right = 0.0 + (range * width/height) / 2.0
            + I * (range) / 2.0;
    } else {
        coords.lower_left = 0.0 - (range) / 2.0
            - I * (range * height/width) / 2.0;
        coords.upper_right = 0.0 + (range) / 2.0
            + I * (range * height/width) / 2.0;
    }

    return coords;
}

complex double
coords_for_pixel(size_t width, size_t height, double range, size_t i, size_t j)
{
    extremes_t extremes = get_extreme_coordinates(width, height, range);
    return horizontal_pixel_to_x_value(extremes, width, i)
        + I * vertical_pixel_to_y_value(extremes, height, j);
}

static size_t
count_escape_time(complex double c)
{
    complex double z =  0.0 + I *  0.0;

    complex double temp;

    for (size_t escape_time = 0; escape_time < MAXIMUM_ITERATIONS; escape_time++) {
        temp = cpow(z, 2);

        if (cabs(temp) > 2)
            return escape_time;

        z = temp + c;
    }

    return 0;
}

static int
choose_color_for_escape_time(size_t escape_time)
{
    if (escape_time == 0)
        return escape_time_colors[1];
    else if (escape_time <= (MAXIMUM_ITERATIONS / 7))
        return escape_time_colors[2];
    else if (escape_time <= (MAXIMUM_ITERATIONS / 5))
        return escape_time_colors[3];
    else
        return escape_time_colors[4];
}

void
draw_something(size_t width, size_t height, double range)
{
    gdImagePtr im = gdImageCreate(width, height);
    FILE *pngout;
    escape_time_colors[0] = gdImageColorAllocate(im, 255, 255, 255);
    escape_time_colors[1] = gdImageColorAllocate(im,   0,   0,   0);
    escape_time_colors[2] = gdImageColorAllocate(im, 176, 229, 247);
    escape_time_colors[3] = gdImageColorAllocate(im, 245, 137, 169);
    escape_time_colors[4] = gdImageColorAllocate(im, 154, 227, 194);

    color_all_pixels(im, width, height, escape_time_colors[0]);
    
    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < height; j++) {
            size_t escape_time = count_escape_time(coords_for_pixel(width, height, range, i, j));
            gdImageSetPixel(im, i, j, choose_color_for_escape_time(escape_time));
        }
    }
    
    pngout = fopen("pngelbrot.png", "wb");
    gdImagePng(im, pngout);
    
    fclose(pngout);
    gdImageDestroy(im);
}
