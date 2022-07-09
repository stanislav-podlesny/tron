#include <osg/Geometry>
#include <osg/Notify>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgGA/StateSetManipulator>
#include <osgViewer/config/SingleWindow>
#include <osgViewer/View>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>



osg::ref_ptr<osg::Geode> createTexturedQuad()
{
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    vertices->push_back(osg::Vec3(-0.5f, 0.0f, -0.5f));
    vertices->push_back(osg::Vec3( 0.5f, 0.0f, -0.5f));
    vertices->push_back(osg::Vec3( 0.5f, 0.0f,  0.5f));
    vertices->push_back(osg::Vec3(-0.5f, 0.0f,  0.5f));

    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));

    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
    texcoords->push_back(osg::Vec2(0.0f, 0.0f));
    texcoords->push_back(osg::Vec2(0.0f, 1.0f));
    texcoords->push_back(osg::Vec2(1.0f, 1.0f));
    texcoords->push_back(osg::Vec2(1.0f, 0.0f));

    osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
    quad->setVertexArray(vertices.get());
    quad->setNormalArray(normals.get());
    quad->setNormalBinding(osg::Geometry::BIND_OVERALL);
    quad->setTexCoordArray(0, texcoords.get());
    quad->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile("test/mipmap/mipmap_level_test-11_levels-bc1.dds");
    texture->setImage(image.get());

    osg::ref_ptr <osg::Geode> geode = new osg::Geode;
    geode->addDrawable(quad.get());
    geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());
    return geode;
}

int main(int argc, char** argv)
{
    osg::setNotifyLevel(osg::DEBUG_FP);  // everything
    
    // build scene graph
    osg::ref_ptr<osg::Group> root = new osg::Group;
    osg::ref_ptr<osg::Geode> textured_quad = createTexturedQuad();
    root->addChild(textured_quad.get());

    // construct the viewer
    osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;

    // add scene graph to viewer
    viewer->setSceneData(root.get());
    
    // windowed
    const int x = 100;
    const int y = 100;
    const int width = 640;
    const int height = 480;
    viewer->apply(new osgViewer::SingleWindow(x, y, width, height));

    // Run at fastest frame rate possible
    // NOTE: "Sync to VBlank" will limit frame rate to that of monitor (60 fps). To turn off:
    //       Start > Nvidia X Server Settigns > OpenGL Settings > [ ] Sync to VBlank
    viewer->setRunFrameScheme(osgViewer::ViewerBase::ON_DEMAND);

    // Set max frame rate to high number
    // viewer->setRunMaxFrameRate(100);

    // turn off swap buffer sync
    // osg::DisplaySettings::instance()->setSyncSwapBuffers(0);

    // add the threading heandler - 'm' key
    viewer->addEventHandler(new osgViewer::ThreadingHandler());

    // add the help handler - 'h' key
    viewer->addEventHandler(new osgViewer::HelpHandler());

    // toggle stats - 's' key
    // stats report to console - 'S' key
    // Frame Time:
    //   Small white ticks are 1ms
    //   Large white ticks are 10ms
    //   Anything over 17ms means you are running below 60 fps
    //   Each white line that goes from Event to GPU is the start of a frame
    //   Most recent frame is on left, then previous frames to the right
    //   Categories
    //     Event:  time processing events (mouse/keyboard/etc.)
    //     Update: time updating scene graph
    //     Cull:   time culling scene graph
    //     Draw:   time drawing scene graph
    //     GPU:    time spent waiting on GPU
    // Graph:
    //   Frame Rate: white
    //   Event:      green
    //   Update:     green
    //   Cull:       cyan
    //   Draw:       yellow
    //   GPU:        orange
    viewer->addEventHandler(new osgViewer::StatsHandler);


    // add the state manipulator
    // 'w' -        wireframe, points, fill
    // 't' -        texturie toggle
    // 'l' -        lighting toggle
    // 'b' -        backface culling toggle
    // 'spacebar' - reset viewing position to home
    viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));

    return viewer->run();
}
