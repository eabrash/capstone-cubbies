
//void writeWorld(std::vector<Model *> models)
//{
//    std::ofstream worldDataStream("output.txt", std::ios::out); // Stream from file
//
////    bench_only.obj    NAME
////    6.0 0.1 0.0       TRANSLATION
////    1.0 1.0 1.0       SCALE
////    0.0 0.0 1.0 0.0   ROTATION
////    1                 1-floor-resting/2-wall-hanging/0-scenery
////    0                 0-single bounding box/1-by mesh bounding boxes
//
//    for (int i = 0; i < models.size(); i++)
//    {
//        worldDataStream << models[i]->getModelFile() << "\n";
//
//        glm::mat4 translation = models[i]->getTranslation();
//        worldDataStream << translation[3][0] << " " << translation[3][1] << " " << translation[3][2] << "\n";
//        glm::mat4 scale = models[i]->getScale();
//        worldDataStream << scale[0][0] << " " << scale[1][1] << " " << scale[2][2] << "\n";
//        glm::mat4 rotation = models[i]->getRotation();
//        worldDataStream << "\n";
//
//        worldDataStream << models[i]->isMovable() << "\n";
//
//        if (models[i]->modelMeshesSplit())
//        {
//            worldDataStream << 1 << "\n";
//        }
//        else
//        {
//            worldDataStream << 0 << "\n";
//        }
//    }
//    worldDataStream.close();
//}
