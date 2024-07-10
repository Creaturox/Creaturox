#include "quadface.h"


QuadFace::QuadFace()
{

}

QuadFace::QuadFace(int v1, int v2, int v3, int v4, int n)
{
    vertNorIndices.resize(5);

    vertNorIndices[0] = v1;
    vertNorIndices[1] = v2;
    vertNorIndices[2] = v3;
    vertNorIndices[3] = v4;
    vertNorIndices[4] = n;

    // make the edge list
    for (int i=0; i<vertNorIndices.length()-1; i++)
    {
        Edge *eg = new Edge();
        if (i==vertNorIndices.length()-2)
        {
            eg->vertexInd_1 = vertNorIndices[i]; eg->vertexInd_2 = vertNorIndices[0];
        }
        else
        {
            eg->vertexInd_1 = vertNorIndices[i]; eg->vertexInd_2 = vertNorIndices[i+1];
        }
        edgeList.append(eg);
    }
}

QuadFace::QuadFace(const QuadFace &faceArg)
{
    vertNorIndices.resize(5);

    for (int i=0; i<faceArg.vertNorIndices.length(); i++)
    {
        vertNorIndices[i] = faceArg.vertNorIndices[i];
    }

    // make the edge list
    for (int i=0; i<vertNorIndices.length()-1; i++)
    {
        Edge *eg = new Edge();
        if (i==vertNorIndices.length()-2)
        {
            eg->vertexInd_1 = vertNorIndices[i]; eg->vertexInd_2 = vertNorIndices[0];
        }
        else
        {
            eg->vertexInd_1 = vertNorIndices[i]; eg->vertexInd_2 = vertNorIndices[i+1];
        }
        edgeList.append(eg);
    }
}

QuadFace::~QuadFace()
{
//    qDebug() << "deleting QuadFace";
    vertNorIndices.clear();
    while(!edgeList.empty()) delete edgeList.last(), edgeList.removeLast();
//    for (int i=0; i<edgeList.length(); i++)
//    {
//        delete edgeList[i];
//    }
//    edgeList.clear();
//    qDebug() << "QuadFace deleted";
}

void QuadFace::cloneQuadFace(QuadFace *qfPr)
{
    for (int i=0; i<this->vertNorIndices.length(); i++)
    {
        qfPr->vertNorIndices.append(this->vertNorIndices[i]);
    }

    // clone all its edges
    for (int i=0; i<this->edgeList.length(); i++)
    {
        Edge *eg = new Edge();
        eg->vertexInd_1 = this->edgeList[i]->vertexInd_1;
        eg->vertexInd_2 = this->edgeList[i]->vertexInd_2;

        qfPr->edgeList.append(eg);
    }
}

