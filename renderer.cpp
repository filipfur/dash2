#include "renderer.h"

#include "glmesh.h"
#include "glframebuffer.h"
#include "gltfloader.h"

#include "settings.h"

#include "gen/shaders/object_vert.h"
#include "gen/shaders/inst_vert.h"
#include "gen/shaders/texture_object_frag.h"
#include "gen/shaders/screen_vert.h"
#include "gen/shaders/bloom_frag.h"
#include "gen/shaders/world_text_vert.h"
#include "gen/shaders/text_border_frag.h"
#include "gen/shaders/sprite_vert.h"
#include "gen/shaders/texture_frag.h"

#include "gen/objects/floor.h"
#include "gen/objects/cube.h"
#include "gen/objects/chess.h"
#include "gen/objects/humanoid.h"

#include "gen/fonts/madimi_one.h"

#include "gen/images/hp_png.h"

#include "hex/extensions.h"

static const lix::Color roseGold{0xFFDDAA};

void Renderer::init()
{
    _instanceShader.reset(new lix::ShaderProgram(assets::shaders::inst_vert,
        assets::shaders::texture_object_frag));
    _objectShader.reset(new lix::ShaderProgram(assets::shaders::object_vert,
        assets::shaders::texture_object_frag));
    _worldTextShader.reset(new lix::ShaderProgram(assets::shaders::world_text_vert,
        assets::shaders::text_border_frag));

    _camera = std::make_shared<Camera>(glm::perspective(glm::radians(80.0f), properties::ASPECT_RATIO, 0.01f, 100.0f));
    _camera->setupUBO({
            _instanceShader.get(),
            _objectShader.get(),
            _worldTextShader.get()
    });
}

auto createMultisampleBuffer()
{
    auto fbo = std::make_shared<lix::FBO>(glm::ivec2{properties::SCREEN_WIDTH, properties::SCREEN_HEIGHT});
    fbo->bind();
    fbo->createRenderBufferMultisample(GL_COLOR_ATTACHMENT0, GL_RGBA16F, 4);
    fbo->createRenderBufferMultisample(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, 4);
    fbo->bindColorsAsDrawBuffers();
    fbo->unbind();
    return fbo;
}

auto createDownsampleBuffer()
{
    auto fbo = std::make_shared<lix::FBO>(glm::ivec2{properties::SCREEN_WIDTH, properties::SCREEN_HEIGHT});
    fbo->bind();
    fbo->createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    fbo->createTexture(GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    fbo->unbind();
    return fbo;
}

void Renderer::render()
{
    //glViewport(0, properties::SCREEN_HEIGHT / 2, properties::SCREEN_WIDTH, properties::SCREEN_HEIGHT / 2);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    static auto msaaFBO = createMultisampleBuffer();
    static auto downsampledFBO = createDownsampleBuffer();
    static auto screenShader = std::make_shared<lix::ShaderProgram>(
        assets::shaders::screen_vert,
        assets::shaders::bloom_frag
    );
    msaaFBO->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    renderInstances();
    renderObjectNodes();
    renderText();
    
    glClear(GL_DEPTH_BUFFER_BIT);
    renderSprites();

    msaaFBO->unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    downsampledFBO->bind();
    msaaFBO->blit(downsampledFBO, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    downsampledFBO->unbind();
    //static lix::BlurPass blurPass{glm::ivec2{properties::SCREEN_WIDTH, properties::SCREEN_HEIGHT}, nullptr};
    //blurPass.blur(downsampledFBO->texture(GL_COLOR_ATTACHMENT1));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    downsampledFBO->bindTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE0);
    //blurPass.outputTexture()->bind(GL_TEXTURE1);
    screenShader->bind();
    screenShader->setUniform("u_bright", 1);
    lix::renderScreen();
}

std::shared_ptr<Camera> Renderer::camera()
{
    return _camera;
}

void Renderer::createFloor(std::vector<std::shared_ptr<lix::Node>>& floorTiles)
{
    static lix::MeshPtr mesh = gltf::loadMesh(assets::objects::floor::Plane_mesh);
    //mesh->material()->setBaseColor(0xFFFFFF);
    //mesh->material()->setBaseColor(roseGold);
    _floorRendering = std::make_shared<lix::InstancedRendering<std::vector<lix::NodePtr>>>(
        mesh,
        floorTiles
    );
}

void Renderer::createTowers(std::vector<std::shared_ptr<lix::TRS>>& pawns)
{
    static lix::MeshPtr mesh = gltf::loadMesh(assets::objects::chess::Tower_mesh);
    //mesh->material()->setBaseColor(0xE4EA95);
    mesh->material()->setBaseColor(roseGold);
    _towerRendering = std::make_shared<TRSInstancedRendering>(
        mesh,
        pawns
    );
}

void Renderer::createPesants(std::vector<std::shared_ptr<lix::TRS>>& pawns)
{
    static lix::MeshPtr mesh = gltf::loadMesh(assets::objects::chess::Peasant_mesh);
    //mesh->material()->setBaseColor(0x56ADAD);
    mesh->material()->setBaseColor(roseGold);
    _pesantRendering = std::make_shared<TRSInstancedRendering>(
        mesh,
        pawns
    );
}

void Renderer::createHorses(std::vector<std::shared_ptr<lix::TRS>>& pawns)
{
    static lix::MeshPtr mesh = gltf::loadMesh(assets::objects::chess::Horse_mesh);
    //mesh->material()->setBaseColor(0xD67CAB);
    mesh->material()->setBaseColor(roseGold);
    _horseRendering = std::make_shared<TRSInstancedRendering>(
        mesh,
        pawns
    );
}

std::shared_ptr<Sprite> Renderer::createHealthBar()
{
    static auto texture = std::make_shared<lix::Texture>(
        assets::images::hp_png_rgba::data,
        assets::images::hp_png_rgba::width,
        assets::images::hp_png_rgba::height,
        GL_UNSIGNED_BYTE,
        GL_RGBA,
        GL_RGBA
    );
    static constexpr float width{512.0f * 0.5f};
    static constexpr float height{128.0f * 0.5f};
    auto hp = std::shared_ptr<Sprite>(
        new Sprite{{glm::vec3{(width - properties::SCREEN_WIDTH) * 0.5f, (properties::SCREEN_HEIGHT - height) * 0.5f, 0.0f}, glm::quat{1.0f, 0.0f, 0.0f, 0.0f}, glm::vec3{width, height, 1.0f}},
        texture}
    );
    _sprites.push_back(hp);
    return hp;
}

void Renderer::renderInstances()
{
    _instanceShader->bind();
    _instanceShader->setUniform("u_curve_factor", Settings::Instance().get("curveFactor"));
    _instanceShader->setUniform("u_n_light_vfx", Settings::Instance().get("nLightVfx"));
    //_instanceShader->setUniform("u_n_light", 0.33f);
    _floorRendering->render(*_instanceShader);
    _towerRendering->render(*_instanceShader);
    _pesantRendering->render(*_instanceShader);
    _horseRendering->render(*_instanceShader);
    //_instanceShader->setUniform("u_n_light", 0.0f);
}

lix::NodePtr Renderer::createPlayer()
{
    static int playerId{1};
    lix::NodePtr playerNode = std::make_shared<lix::Node>();
    playerNode->setName("Player_" + std::to_string(playerId++));
    //playerNode->appendChild(gltf::loadNode(assets::objects::humanoid::Cube_node));
    playerNode->appendChild(gltf::loadNode(assets::objects::cube::Cube_node));
    //playerNode->children().front()->mesh()->material()->setBaseColor(0x00);
    _objectNodes.push_back(playerNode);
    return playerNode;
}

void Renderer::renderObjectNodes()
{
    _objectShader->bind();
    _objectShader->setUniform("u_curve_factor", Settings::Instance().get("curveFactor"));
    _objectShader->setUniform("u_n_light_vfx", Settings::Instance().get("nLightVfx"));
    for(const auto& node : _objectNodes)
    {
        lix::renderNode(*_objectShader, *node, true, true);
    }
}

void Renderer::renderText()
{
    _worldTextRendering->render();
}

void Renderer::renderSprites()
{
    static lix::ShaderProgram spriteShader{
        assets::shaders::sprite_vert,
        assets::shaders::texture_frag
    };
    static glm::mat4 orthoProj{glm::ortho(-properties::SCREEN_WIDTH * 0.5f, properties::SCREEN_WIDTH * 0.5f,
        -properties::SCREEN_HEIGHT * 0.5f, properties::SCREEN_HEIGHT * 0.5f)};
    static glm::mat4 view{glm::lookAt(glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f})};
    spriteShader.bind();
    spriteShader.setUniform("u_projection", orthoProj);
    spriteShader.setUniform("u_view", view);
    static float ctr{0.0f};

    static lix::VertexArray spriteVAO{
        {
            lix::VEC2, lix::VEC2
        },
        {
            -0.5f, -0.5f, 0.0f, 0.0f,
            +0.5f, -0.5f, 1.0f, 0.0f,
            +0.5f, +0.5f, 1.0f, 1.0f,
            -0.5f, +0.5f, 0.0f, 1.0f},
        {
            0, 1, 2,
            3, 0, 2
        },
        GL_TRIANGLES,
        GL_STATIC_DRAW
    };

    spriteVAO.bind();
    for(const auto& sprite : _sprites)
    {
        if(sprite->visible)
        {
            sprite->texture->bind(GL_TEXTURE0);
            int t = hex::as<int>(ctr += 0.01f);
            spriteShader.setUniform("u_sprite_uv", sprite->uvWindow);
            spriteShader.setUniform("u_model", sprite->trs.model());
            spriteVAO.draw();
        }
    }
}

void Renderer::createWorldTexts(std::vector<std::shared_ptr<lix::Text>>& worldTexts)
{
    static std::shared_ptr<lix::Font> font = std::make_shared<lix::Font>(
        assets::fonts::madimi_one::create()
    );
    static auto textProps = lix::Text::PropBuilder()
        .setTextColor(lix::Color::white)
        .setTextScale(0.004f)
        .setAlignment(lix::Text::CENTER)
        .setBorderColor(lix::Color::black);
    for(const auto& p : properties::DIRECTIONS)
    {
        glm::vec3 pn = glm::normalize(p);
        std::shared_ptr<lix::Text> text;
        text.reset(new lix::Text(font, textProps, "Test"));
        text->setTranslation(pn * 1.5f + glm::vec3{0.0f, 1.0f + pn.z * 0.3f, 0.0f});
        text->setRotation(glm::angleAxis(glm::radians(180.0f), glm::vec3{0.0f, 0.0f, 1.0f})
         * glm::angleAxis(-glm::radians(45.0f), glm::vec3{1.0f, 0.0f, 0.0f}));
        worldTexts.push_back(text);
    }

    std::shared_ptr<lix::Text> editText;
    editText.reset(new lix::Text(font, textProps, ""));
    editText->setTranslation(glm::vec3{0.0f, 1.0f, 0.0f});
    editText->setRotation(glm::angleAxis(glm::radians(180.0f), glm::vec3{0.0f, 0.0f, 1.0f})
        * glm::angleAxis(-glm::radians(45.0f), glm::vec3{1.0f, 0.0f, 0.0f}));
    editText->setScale(glm::vec3{1.5f});
    worldTexts.push_back(editText);

    _worldTextRendering.reset( new lix::TextRendering(
        properties::SCREEN_RESOLUTION, 
        worldTexts,
        _worldTextShader
    ));
}

std::shared_ptr<NodeInstancedRendering> Renderer::floorRendering()
{
    return _floorRendering;
}
std::shared_ptr<TRSInstancedRendering> Renderer::towerRendering()
{
    return _towerRendering;
}
std::shared_ptr<TRSInstancedRendering> Renderer::pesantRendering()
{
    return _pesantRendering;
}
std::shared_ptr<TRSInstancedRendering> Renderer::horseRendering()
{
    return _horseRendering;
}
std::shared_ptr<lix::TextRendering> Renderer::worldTextRendering()
{
    return _worldTextRendering;
}