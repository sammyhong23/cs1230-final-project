#ifndef FLOWMAP_H
#define FLOWMAP_H


class FlowMap
{
public:
    FlowMap();
    void passFlowMapData();
    void passMainTextureData();

private:
    const float scrollSpeed = 1.0f;
    const float scrollDir = 1.0f;
};

#endif // FLOWMAP_H
