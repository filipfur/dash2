#include "app.h"

#include "noise.h"
#include "settings.h"

App::App() : lix::Application{properties::SCREEN_WIDTH, properties::SCREEN_HEIGHT, "Lithium X - Example"},
    _grid{glm::vec2{1.0f}, properties::MAP_CELLS}, _game{_grid}, _pawns{}
{
    _game.setIGame(this);
}

void App::init()
{
    srand(6241246);

    setInputAdapter(this);

    _renderer.reset(new Renderer());
    _renderer->init();

    auto playerNode = _renderer->createPlayer();
    auto camera = _renderer->camera();
    playerNode->appendChild(camera);
    playerNode->setTranslation(glm::vec3{0.0f, 0.0f, -properties::MAP_CELLS.y * 0.5f});
    Player& pl = _game.createPlayer(playerNode);
    _renderer->createWorldTexts(_worldTexts);
    _game.makeActivePlayer(pl);
    
    camera->setTranslation(glm::vec3{0.0f, 4.0f, -2.0f});

    static std::vector<std::shared_ptr<lix::Node>> floorTiles;
    if(floorTiles.empty())
    {
        for(int i{0}; i < properties::MAP_CELLS.y; ++i)
        {
            for(int j{0}; j < properties::MAP_CELLS.x; ++j)
            {
                glm::vec3 wp = _grid.toWorldCoordinate(glm::ivec2{j, i});
                floorTiles.emplace_back(new lix::Node(wp));

                glm::vec3 d = playerNode->translation() - wp;
                if(d.x * d.x + d.z * d.z < 8.0f)
                {
                    continue;
                }

                //int r = rand() % 2;
                //if(glm::abs(noise::simplex2d(wp.x * 2.0f, wp.z * 2.0f)) > 0.4f)

                int likely{1 + 3 * (hex::as<int>(_pawns[i - 1][j] != nullptr)
                    + hex::as<int>(_pawns[i][j - 1] != nullptr)
                    + hex::as<int>(_pawns[i - 1][j - 1] != nullptr))};

                if(!(i > 0 && j > 0 && i < (properties::MAP_CELLS.y - 1) && j < (properties::MAP_CELLS.x - 1)))
                {
                    continue;
                }

                if((rand() % likely) == 0)
                {
                    int r = rand() % 12;
                    //int r = glm::round(glm::abs(noise::simplex2d(wp.x * 128.0f, wp.z * 128.0f)) * 6.0f);
                    auto pawn = std::make_shared<Pawn>();
                    switch(r)
                    {
                        case 0:
                            pawn->type = Pawn::TOWER;
                            break;
                        case 1:
                        case 2:
                        case 3:
                            pawn->type = Pawn::HORSE;
                            break;
                        default:
                            pawn->type = Pawn::PEASANT;
                            break;
                    }
                    auto* pawns = &_pesants;
                    if(pawn->type == Pawn::TOWER)
                    {
                        pawns = &_towers;
                    }
                    else if(pawn->type == Pawn::HORSE)
                    {
                        pawns = &_horses;
                    }
                    static uint32_t nextId{0};
                    pawn->id = nextId++;
                    pawn->trs = pawns->emplace_back(new lix::TRS(wp));
                    _pawns[i][j] = pawn;
                }
            }
        }
    }
    _renderer->createFloor(floorTiles);
    _renderer->createTowers(_towers);
    _renderer->createPesants(_pesants);
    _renderer->createHorses(_horses);

    _currentSetting = Settings::Instance().begin();

    auto textNode = std::make_shared<lix::Node>();
    for(const auto& node : _worldTexts)
    {
        textNode->appendChild(node);
    }
    playerNode->appendChild(textNode);

    setOnKeyDown(SDLK_ESCAPE, [this](auto,auto) {
        quit();
    });

    setOnKeyDown(SDLK_RIGHT, [this](auto,auto) {
        if(++_currentSetting == Settings::Instance().end())
        {
            _currentSetting = Settings::Instance().begin();
        }
        std::cout << "Setting: " << _currentSetting->first << std::endl;
    });
    setOnKeyDown(SDLK_LEFT, [this](auto,auto) {
        if(_currentSetting == Settings::Instance().begin())
        {
            _currentSetting = Settings::Instance().end();
        }
        --_currentSetting;
        std::cout << "Setting: " << _currentSetting->first << std::endl;
    });

    setOnKeyDown(SDLK_UP, [this](auto,auto) {
        float val = Settings::Instance().get(_currentSetting->first) + 0.01f;
        Settings::Instance().set(_currentSetting->first, val);
        std::cout << val << std::endl;
    });
    setOnKeyDown(SDLK_DOWN, [this](auto,auto) {
        float val = Settings::Instance().get(_currentSetting->first) - 0.01f;
        Settings::Instance().set(_currentSetting->first, val);
        std::cout << val << std::endl;
    });

    for(size_t i{0}; i < properties::NUM_DIRECTIONS; ++i)
    {
        _worldTexts[i]->setText(_game.getRandomWord());
    }

    _hpSprite = _renderer->createHealthBar();
}

void App::tick(float dt)
{
    auto camera = _renderer->camera();
    static glm::vec3 cameraTarget{0.0f, 0.0f, 0.0f};
    _game.tick(dt);

    _hpSprite->uvWindow = glm::vec4{1.0f, 0.2f, 0.0f, 0.2f * std::max(0, _game.currentPlayer()->health - 1)};
    if(_game.currentPlayer()->health <= 0)
    {
        _hpSprite->visible = false;
    }

    //cameraTarget = glm::mix(cameraTarget, _game.currentPlayer()->node->translation(), dt * 10.0f);
    //camera->setTarget(cameraTarget);
    camera->setTarget(camera->position() - camera->translation());
    camera->refresh(dt);
}

void App::draw()
{
    _renderer->render();
}

void App::onMouseDown(lix::KeySym key, lix::KeyMod mod)
{

}

void App::onMouseUp(lix::KeySym key, lix::KeyMod mod)
{

}

void App::onKeyDown(lix::KeySym key, lix::KeyMod mod)
{
    static std::string inputText;
    char c = static_cast<char>('a' + (key - SDLK_a));
    //c = std::max('a', std::min(c, 'z'));
    if(key == SDLK_BACKSPACE && inputText.length() > 0)
    {
        inputText.erase(inputText.end() - 1);
    }
    else
    {
        if(c < 'a' || c > 'z')
        {
            return;
        }
        inputText += c;
        for(size_t i{0}; i < properties::NUM_DIRECTIONS; ++i)
        {
            const auto& text = _worldTexts.at(i);
            if(text->text().length() == inputText.length()
                && std::equal(inputText.begin(), inputText.end(), text->text().begin()))
            {
                text->setText(_game.getRandomWord());
                inputText.clear();
                _worldTexts.back()->setText("");
                ActivePlayer& activePlayer = _game.activePlayer(_game.currentPlayer()->id);
                activePlayer.setTarget(activePlayer.target() + properties::DIRECTIONS[i]);
                activePlayer.transition_to(activePlayer.moving);
                //_game.currentPlayer()->velocity = properties::DIRECTIONS[i];
                //_game.currentPlayer()->target += properties::DIRECTIONS[i];
                _game.currentPlayer()->targetRotation = glm::angleAxis(glm::pi<float>() * 0.5f - i * glm::pi<float>() * 0.25f, glm::vec3{0.0f, 1.0f, 0.0f});
                auto pawnA = pawnAt(activePlayer.target());
                if(pawnA)
                {
                    _game.makeActivePawn(pawnA);
                    removeFromGrid(pawnA->trs->translation());
                    activePlayer.setTarget(activePlayer.target() + properties::DIRECTIONS[i]);
                    auto pawnB = pawnAt(activePlayer.target());
                    while(pawnB)
                    {
                        _game.makeActivePawn(pawnB);
                        removeFromGrid(pawnB->trs->translation());
                        pawnA = pawnB;
                        activePlayer.setTarget(activePlayer.target() + properties::DIRECTIONS[i]);
                        pawnB = pawnAt(activePlayer.target());
                    }
                }

                return;
            }
        }
    }
    _worldTexts.back()->setText(inputText);
}

void App::onKeyUp(lix::KeySym key, lix::KeyMod mod)
{
    
}

std::shared_ptr<Pawn> App::pawnAt(const glm::vec3& wp)
{
    auto cellId = _grid.toCellId(wp);
    return _pawns[cellId.y][cellId.x];
}

void App::removeFromGrid(const glm::vec3& wp)
{
    auto cellId = _grid.toCellId(wp);
    _pawns[cellId.y][cellId.x].reset();
}

bool App::tryMoveOnGrid(const glm::vec3& p0, const glm::vec3& p1)
{
    auto c0 = _grid.toCellId(p0);
    auto c1 = _grid.toCellId(p1);
    if(_pawns[c1.y][c1.x])
    {
        return false;
    }
    _pawns[c1.y][c1.x] = std::move(_pawns[c0.y][c0.x]); 
    _pawns[c0.y][c0.x].reset();
    return true;
}

void App::deletePawn(std::shared_ptr<Pawn> pawn)
{
    auto pawns = &_pesants;
    switch(pawn->type)
    {
        case Pawn::TOWER:
            pawns = &_towers;
            break;
        case Pawn::HORSE:
            pawns = &_horses;
            break;
        default:
            break;
    }
    pawns->erase(std::remove_if(pawns->begin(), pawns->end(), [&pawn](auto p) {
        return p.get() == pawn->trs.get();
    }));
}