#pragma once 
#include <comutil.h>  // ��� ������ � BSTR
#include <vector>
#include <array>
using namespace std;

class PipelineSolver {
public:
    PipelineSolver(array<double, 3> userStartPoint = {NAN, NAN, NAN});  // �����������

    vector <array<double, 8>> PipelineSolver::getPipelineTrajectory();

private:
    array<double, 3> start_point;  // ���������� ��� ������ �����
    vector<array<double, 3>> pipeline_dots;  // ������ �����
    vector<wstring> macro_objects_names; // ������ � ������� �������� �����
    array<double, 3> zero_point;
    vector<array<double, 3>> zero_dots_coords;  // ������ � ������ ���������
    vector<array<double, 6>> lines_coords;  // ������ ��������� �����
    vector <array <double, 3>> relativeCoordinates; //������ ��������� �������������� �������� �����
    double bendRadius = 75; // �������� ������� ���� �����
    vector < array<double, 3>> bendParams;
    vector <array<double, 8>> bendInfo;

    void getMacroObjectsNames();
    bool arePointsEqual(const array<double, 3>& p1, const array<double, 3>& p2, double epsilon);
    void getLinesCoords();  // ����� ��� ��������� ��������� ���� �����
    void findPipeline();  // ����������� ������� ��� ���������� �������
    void getZeroDotsCoords(); // ������� ���������� ����� � ������� ����������
    void getRelativePointCoordinates(); //������� ����������� �������������� �������� �����
    void calculateBendParams();
    void formBendInformation();
    double calculateDistance(const array <double, 3> &p1, const array <double, 3> &p2);
    double calculateAngleBetweenVectors(const array<double, 3>& v1, const array<double, 3>& v2);
    double calculateAngleBetweenPlanes(const array<double, 3>& n1, const array<double, 3>& n2);
    array <double, 3> getCrossProduct(const array<double, 3>& v1, const array<double, 3>& v2);
    array <double, 3> normalizeVector(const array<double, 3>& v);
};

