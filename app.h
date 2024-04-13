#pragma once

#include <memory>
#include <set>

#include "glapplication.h"
#include "glnode.h"
#include "properties.h"
#include "renderer.h"
#include "game.h"
#include "glinputadapter.h"
#include "json.h"
#include "grid.h"
#include "pawn.h"
#include "igame.h"

class App : public lix::Application, public lix::InputAdapter, public IGame
{
public:
    App();
    virtual void init() override;
    virtual void tick(float dt) override;
    virtual void draw() override;

    virtual void onMouseDown(lix::KeySym key, lix::KeyMod mod) override;
    virtual void onMouseUp(lix::KeySym key, lix::KeyMod mod) override;
    virtual void onKeyDown(lix::KeySym key, lix::KeyMod mod) override;
    virtual void onKeyUp(lix::KeySym key, lix::KeyMod mod) override;

    virtual std::shared_ptr<Pawn> pawnAt(const glm::vec3& wp) override;
    virtual void removeFromGrid(const glm::vec3& wp) override;
    virtual bool tryMoveOnGrid(const glm::vec3& p0, const glm::vec3& p1) override;
    virtual void deletePawn(std::shared_ptr<Pawn> pawn) override;

private:
    std::shared_ptr<Renderer> _renderer;
    std::vector<std::shared_ptr<lix::Text>> _worldTexts;
    Grid _grid;
    Game _game;
    std::vector<std::shared_ptr<lix::TRS>> _towers;
    std::vector<std::shared_ptr<lix::TRS>> _pesants;
    std::vector<std::shared_ptr<lix::TRS>> _horses;
    std::set<std::shared_ptr<Pawn>> _attackingPawns;
    std::map<std::string,json::Json>::const_iterator _currentSetting;
    std::shared_ptr<Pawn> _pawns[properties::MAP_CELLS.y][properties::MAP_CELLS.x];
    std::shared_ptr<Sprite> _hpSprite;
};