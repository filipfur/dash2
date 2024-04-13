#pragma once

#include <memory>
#include <vector>

#include "glshaderprogram.h"
#include "camera.h"
#include "gltextrendering.h"
#include "glinstancedrendering.h"
#include "glnode.h"

#include "sprite.h"
#include "properties.h"

using TRSInstancedRendering = lix::InstancedRendering<std::vector<std::shared_ptr<lix::TRS>>>;
using NodeInstancedRendering = lix::InstancedRendering<std::vector<std::shared_ptr<lix::Node>>>;

class Renderer
{
public:

    void init();

    void render();

    std::shared_ptr<Camera> camera();
    lix::NodePtr createPlayer();
    void createWorldTexts(std::vector<std::shared_ptr<lix::Text>>& worldTexts);
    void createFloor(std::vector<std::shared_ptr<lix::Node>>& floorTiles);
    void createTowers(std::vector<std::shared_ptr<lix::TRS>>& pawns);
    void createPesants(std::vector<std::shared_ptr<lix::TRS>>& pawns);
    void createHorses(std::vector<std::shared_ptr<lix::TRS>>& pawns);

    std::shared_ptr<Sprite> createHealthBar();

    std::shared_ptr<NodeInstancedRendering> floorRendering();
    std::shared_ptr<TRSInstancedRendering> towerRendering();
    std::shared_ptr<TRSInstancedRendering> pesantRendering();
    std::shared_ptr<TRSInstancedRendering> horseRendering();
    std::shared_ptr<lix::TextRendering> worldTextRendering();

private:
    void renderObjectNodes();
    void renderInstances();
    void renderText();
    void renderSprites();

    std::shared_ptr<Camera> _camera;

    std::shared_ptr<lix::ShaderProgram> _objectShader;
    std::shared_ptr<lix::ShaderProgram> _instanceShader;
    std::shared_ptr<lix::ShaderProgram> _worldTextShader;
    std::vector<std::shared_ptr<lix::Node>> _objectNodes;

    std::shared_ptr<NodeInstancedRendering> _floorRendering;
    std::shared_ptr<TRSInstancedRendering> _towerRendering;
    std::shared_ptr<TRSInstancedRendering> _pesantRendering;
    std::shared_ptr<TRSInstancedRendering> _horseRendering;
    std::shared_ptr<lix::TextRendering> _worldTextRendering;

    std::vector<std::shared_ptr<Sprite>> _sprites;
};