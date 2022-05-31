#include <iostream>
#include <Standard_Handle.hxx>
#include <StepToGeom.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <vector>
#include <TopoDS_Shape.hxx>

#include <map>
#include <filesystem>
#include <Interface_Static.hxx>
#include "Utils.h"

using namespace std;

map<string,int> edgeType_count, faceType_count;
TopoDS_Shape ReadStep(string);
void ExportSTEP(const TopoDS_Shape&, const string&, const string&);
void Stats_TopoShapes(const TopoDS_Shape &shape, bool = true);
void StepFolder_Stats(string=string(SRCDIR)+"/step_files");
void taper(const TopoDS_Shape&, Handle(Geom_Plane),double);

const char *shape_types[9] = {
        "TopAbs_COMPOUND",
        "TopAbs_COMPSOLID",
        "TopAbs_SOLID",
        "TopAbs_SHELL",
        "TopAbs_FACE",
        "TopAbs_WIRE",
        "TopAbs_EDGE",
        "TopAbs_VERTEX",
        "TopAbs_SHAPE"
};

const char *faceContinuity[7] = {
        "GeomAbs_C0",
        "GeomAbs_G1",
        "GeomAbs_C1",
        "GeomAbs_G2",
        "GeomAbs_C2",
        "GeomAbs_C3",
        "GeomAbs_CN"
};

int main(int argc, char** argv) {

    //read step file
    //TopoDS_Shape shape = ReadStep("/home/tom/Documents/stage_can/occ_test/Cone_surf.stp");

    // compute stats
    //Stats_TopoShapes(shape);

    // export TopoDS to step file
    //ExportSTEP(shape, "out.step", "mm");

    StepFolder_Stats();
}


void Stats_TopoShapes(const TopoDS_Shape& shape, bool verbose){
    Standard_Integer count(0);
    std::vector<Geom_Surface> surfs;

    // get underlying buffer
    streambuf* orig_buf = cout.rdbuf();

    // set null
    if(!verbose)cout.rdbuf(NULL);


    cout << "--------------- COUNT OF SHAPES OF != TYPES : ---------------" << endl;
    cout << "/////////////////////////////////////////////////////////////" << endl;
    for(int shape_enum_value = 0; shape_enum_value != TopAbs_SHAPE; shape_enum_value++){
        count = 0;
        TopAbs_ShapeEnum ev = static_cast<TopAbs_ShapeEnum>(shape_enum_value);
        for(TopExp_Explorer explorer(shape, ev); explorer.More(); explorer.Next()){
            count++;
        }
        cout << "nb of " << shape_types[shape_enum_value] << " is " << count << endl;
    }

    cout << "--------------------------- FACES TYPES ---------------------" << endl;
    cout << "/////////////////////////////////////////////////////////////" << endl;
    count = 0;
    for(TopExp_Explorer explorer(shape, TopAbs_FACE); explorer.More(); explorer.Next()){
        count++;
        const opencascade::handle<Geom_Surface> &surface = BRep_Tool::Surface(TopoDS::Face((explorer.Current())));
        //cout << "face " << count << " , continuity " << faceContinuity[surface->Continuity()] << endl;
        //cout << typeid(surface).name() << endl;
        Standard_CString type = surface->DynamicType()->Name();
        cout << "face " << count << " : "<< type << endl;
        faceType_count.contains(type) ? faceType_count[type] += 1 : faceType_count[type] = 1;
    }

    cout << "------------------ EDGES TYPES ------------------------------" << endl;
    cout << "/////////////////////////////////////////////////////////////" << endl;
    count = 0;
    for(TopExp_Explorer explorer(shape, TopAbs_EDGE); explorer.More(); explorer.Next()){
        count++;
        Standard_Real first(0.0), last(1.0);
        const opencascade::handle<Geom_Curve> &curve = BRep_Tool::Curve(TopoDS::Edge(explorer.Current()),first,last);
        if(!curve.IsNull()){
            Standard_CString type = curve->DynamicType()->Name();
            cout << "edge " << count << " : "<< type << endl;
            edgeType_count.contains(type) ? edgeType_count[type] += 1 : edgeType_count[type] = 1;
        }
    }

    cout << "--------------- COUNT OF TYPES  : ---------------------------" << endl;
    cout << "/////////////////////////////////////////////////////////////" << endl;
    cout << "--------------------------- FACES ---------------------------" << endl;
    for (auto it = faceType_count.begin(); it!= faceType_count.end(); ++it){
        cout << it->first << " " << it->second << endl;
    }
    cout << "--------------------------- EDGES ---------------------------" << endl;
    for (auto it = edgeType_count.begin(); it!= edgeType_count.end(); ++it){
        cout << it->first << " " << it->second << endl;
    }
    cout.rdbuf(orig_buf);
}

namespace fs = std::filesystem;


/**
 * Compute step files stats and store them in a csv file
 * @param folder_path folder path containing step files
 */
void StepFolder_Stats(string folder_path) {
    //init
    cout << "------------------------------------------------------" << endl;
    cout << "Computing stats from step files in folder : " << endl;
    cout << folder_path << endl;
    std::ofstream stats;
    faceType_count.clear();
    edgeType_count.clear();

    //computing stats
    cout << "------------------------------------------------------" << endl;
    int file_count(0);
    for(auto& p: fs::directory_iterator(folder_path))++file_count;

    for(auto& p: fs::directory_iterator(folder_path)){
        cout << p.path().filename() << '\n';
        Stats_TopoShapes(ReadStep(p.path()), false);
    }
    map<string,int> totFace, totEdge;
    totFace.insert(faceType_count.begin(),faceType_count.end());
    totEdge.insert(edgeType_count.begin(),edgeType_count.end());


    //storing stats
    string stat_file(string(SRCDIR)+"/step_folder_stat.csv");
    stats.open(stat_file);

    string firstLine ("File/Geom_TYPE,");
    for(auto it = totFace.begin(); it != totFace.end(); ++it) firstLine += string(it->first) + ",";
    for(auto it = totEdge.begin(); it != totEdge.end(); ++it) firstLine += string(it->first) + ",";

    firstLine.pop_back();
    stats << firstLine << endl;

    int i=0;
    for(auto& p: fs::directory_iterator(folder_path)){
        for (auto it = faceType_count.begin(); it!=faceType_count.end(); ++it) it->second=0;
        for (auto it = edgeType_count.begin(); it!=edgeType_count.end(); ++it) it->second=0;
        Stats_TopoShapes(ReadStep(p.path()), false);
        //string line( string(p.path().filename()) + ",");
        std::ostringstream file_number;
        file_number << std::setw(to_string(file_count).size() + 1) << std::setfill('0') << ++i;
        string line( "F" +file_number.str() + ",");
        for (auto it = faceType_count.begin(); it!=faceType_count.end(); ++it) line += to_string(it->second) + ",";
        for (auto it = edgeType_count.begin(); it!=edgeType_count.end(); ++it) line += to_string(it->second) + ",";
        line.pop_back();
        stats << line << endl;
    }

    string line( "TOTAL,");
    for(auto it = totFace.begin(); it != totFace.end(); ++it) line += to_string(it->second) + ",";
    for(auto it = totEdge.begin(); it != totEdge.end(); ++it) line += to_string(it->second) + ",";
    line.pop_back();
    stats << line << endl;

    stats.close();
    cout << "------------------------------------------------------" << endl;
    cout << "Stats writen to :\n" << stat_file << endl;
}

void taper(const TopoDS_Shape &shape, opencascade::handle<Geom_Plane> plane, double angle) {
    //TODO

}