#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtDebug>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Notify>
#include <osg/Point>
#include <osg/State>
#include <osg/TexGen>
#include <osg/Texture2D>
#include <osg/ShapeDrawable>
#include <osgUtil/DelaunayTriangulator>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgGA/StateSetManipulator>

#include <osgSim/Impostor>

#include <osgViewer/config/SingleWindow>
#include <osgViewer/View>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>


// Select higest preforming graphics card
// https://stackoverflow.com/questions/6036292/select-a-graphic-device-in-windows-opengl
// extern "C" {
//     _declspec(dllexport) int32_t NvOptimusEnablement = 1;
//     _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
// }

// TODO: convert to binary file
// TODO: optimizations for 60fps full map
// TODO: Make this reproducible: argoverse2 -> cities in osg binary
// TODO: Get natvis file for Qt, make QString work in debugger. "Use "visualizerFile" and "showDisplayString" in launch.vs.json https://stackoverflow.com/questions/58624914/using-natvis-file-to-visualise-c-objects-in-vs-code
// TODO: remove dup verts (average?)
// TODO: save as single vertex array...can we do that with osg::PagedLOD?
// TODO: Report compiling hello_cmake.cpp gives this error: "Unable to find compilation information for this file". Does it work with makefile?
// TODO: Use different number formats (short, int?) to see if it speeds up map
// TODO: Use osg::PagedLOD per tile
// TODO: Use osg::ProxyNode

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    const QString city = "austin"; // austin, dearborn, miami, palo-alto, pittsburgh, washington-dc 
    
    QDir osgb_dir(QDir::homePath() + "/Downloads/vector_map/cities/" + city);
    osgb_dir.setFilter(QDir::Files);
    if (!osgb_dir.exists()) 
    {
        qFatal("Expected directory does not exist: %s", qPrintable(osgb_dir.absolutePath()));
    }

    osg::ref_ptr<osg::Group> root = new osg::Group;
    
    for (const QFileInfo& osgb_fi : osgb_dir.entryInfoList())
    {
        osg::Object* obj = osgDB::readObjectFile(osgb_fi.absoluteFilePath().toStdString());
        osg::Node* node = dynamic_cast<osg::Node*>(obj);
        root->addChild(node);
    }
    
    // show all messages
    // osg::setNotifyLevel(osg::ALWAYS);
    // osg::setNotifyLevel(osg::WARN);
    osg::setNotifyLevel(osg::INFO);
    // osg::setNotifyLevel(osg::DEBUG_FP);

    // construct the viewer.
    osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;

    // Run at fastest frame rate possible
    // NOTE: "Sync to VBlank" will limit frame rate to that of monitor (60 fps). To turn off:
    //       Start > Nvidia X Server Settigns > OpenGL Settings > [ ] Sync to VBlank
    // viewer->setRunFrameScheme(osgViewer::ViewerBase::CONTINUOUS);
    viewer->setRunFrameScheme(osgViewer::ViewerBase::ON_DEMAND);

    // Set max frame rate to high number
    // viewer->setRunMaxFrameRate(100);

    // turn off swap buffer sync
    // osg::DisplaySettings::instance()->setSyncSwapBuffers(0);


    // add model to viewer.
    viewer->setSceneData(root.get());

    // add the help handler
    viewer->addEventHandler(new osgViewer::HelpHandler());

    // 's' - toggle stats
    auto stats = new osgViewer::StatsHandler;
    viewer->addEventHandler(stats);

    // windowed
    const int x = 100;
    const int y = 100;
    const int width = 640;
    const int height = 480;
    viewer->apply(new osgViewer::SingleWindow(x, y, width, height));

    // add the state manipulator.
    // 'w' - wireframe, points, fill
    // 't' - texturie toggle
    // 'l' - lighting toggle
    // 'b' - backface culling toggle
    viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));


    osgViewer::Viewer::ThreadingModel threadingModel = osgViewer::Viewer::CullThreadPerCameraDrawThreadPerContext;
        // while (arguments.read("-s")) { threadingModel = osgViewer::Viewer::SingleThreaded; }
        // while (arguments.read("-g")) { threadingModel = osgViewer::Viewer::CullDrawThreadPerContext; }
        // while (arguments.read("-d")) { threadingModel = osgViewer::Viewer::DrawThreadPerContext; }
        // while (arguments.read("-c")) { threadingModel = osgViewer::Viewer::CullThreadPerCameraDrawThreadPerContext; }
    viewer->setThreadingModel(threadingModel);

    return viewer->run();
}
