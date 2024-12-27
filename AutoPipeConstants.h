#pragma once

enum gabaritType {
	GABARIT_SIZE = 1,
	GABARIT_POINTS = 2
};

struct ViewGabarits
{
	double x_gab = NAN, y_gab = NAN; //размеры габаритного прямоугольника
	double x_bot = NAN, y_bot = NAN; //координаты нижней левой точки
	double x_top = NAN, y_top = NAN; //координаты верхней правой точки
};