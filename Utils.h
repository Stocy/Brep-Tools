#ifndef OCC_TEST_UTILS_H
#define OCC_TEST_UTILS_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <STEPControl_Writer.hxx>
#include <STEPControl_Reader.hxx>

static int test(int);

using namespace std;
static bool cmp(pair<string, int>& a,
         pair<string, int>& b)
{
    return a.second > b.second;
}

static vector<pair<string,int>> sort_map(map<string, int>& M)
{
    vector<pair<string, int> > A;
    for (auto& it : M) {
        A.push_back(it);
    }
    sort(A.begin(), A.end(), cmp);
    return A;
}

/**
 * Read a step file from a path
 * @return TopoDS_Shape shape with all roots
 */
static TopoDS_Shape ReadStep(string path) {
    STEPControl_Reader reader;
    IFSelect_ReturnStatus stat = reader.ReadFile(path.c_str());
    IFSelect_PrintCount mode = IFSelect_ListByItem;
    //reader.PrintCheckLoad(false, mode);

    Standard_Integer NbRoots = reader.NbRootsForTransfer();                      //Transfer whole file
    Standard_Integer num = reader.TransferRoots();
    Standard_Integer NbTrans = reader.TransferRoots();

    TopoDS_Shape result = reader.OneShape();
    return result;
}

/**
 * Export TopoDS_Shape in STEP file with filename
 * @param shape
 * @param filename
 * @param unit
 */
static void ExportSTEP(const TopoDS_Shape& shape, const string& filename, const string& unit) {
    cout << "------------------ exporting STEP in "<< filename << "--------------------"<< endl;
    if (shape.IsNull()) {
        throw new invalid_argument("Can't export null shape to STEP");
    }
    STEPControl_Writer writer;
    Interface_Static::SetCVal ("xstep.cascade.unit", unit.c_str());
    Interface_Static::SetCVal ("write.step.unit", unit.c_str ());
    Interface_Static::SetIVal ("write.step.nonmanifold", 1);
    // "Transfer" = convert
    IFSelect_ReturnStatus transferStatus = writer.Transfer(shape, STEPControl_AsIs);

    if (transferStatus != IFSelect_RetDone) {
        throw std::logic_error ("Error while transferring shape to STEP");
    }
    // Write transferred structure to STEP file
    IFSelect_ReturnStatus writeStatus = writer.Write(filename.c_str());

    // Return previous locale
    if (writeStatus != IFSelect_RetDone)
    {
        throw std::logic_error ("Error while writing transferred shape to STEP file");
    }
}


#endif //OCC_TEST_UTILS_H
