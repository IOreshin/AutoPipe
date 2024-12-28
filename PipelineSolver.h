#pragma once  // ������ �� ��������� ���������

#include <vector>
#include <array>
using namespace std;

class PipelineSolver {
public:
    PipelineSolver(array<double, 3> userStartPoint = {NAN, NAN, NAN});  // �����������

    vector <array<double, 3>> PipelineSolver::getPipelineTrajectory();

private:
    array<double, 3> start_point;  // ���������� ��� ������ �����
    vector<array<double, 3>> pipeline_dots;  // ������ �����
    vector<array<double, 6>> lines_coords;  // ������ ��������� �����
    vector <array <double, 3>> relativeCoordinates; //������ ��������� �������������� �������� �����

    void getLinesCoords();  // ����� ��� ��������� ��������� ���� �����
    void findPipeline();  // ����������� ������� ��� ���������� �������
    void getRelativePointCoordinates(); //������� ����������� �������������� �������� �����
};

