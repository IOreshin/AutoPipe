#pragma once

enum gabaritType {
	GABARIT_SIZE = 1,
	GABARIT_POINTS = 2
};

struct ViewGabarits
{
	double x_gab = NAN, y_gab = NAN; //������� ����������� ��������������
	double x_bot = NAN, y_bot = NAN; //���������� ������ ����� �����
	double x_top = NAN, y_top = NAN; //���������� ������� ������ �����
};