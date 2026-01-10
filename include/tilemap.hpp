#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cstdint>

class TileMap : public sf::Drawable, public sf::Transformable {
public:
    using TileId = std::uint16_t;

    bool loadAtlas(const std::string& atlasPath, sf::Vector2u tileSizePx);

    // NOWE: atlas z wielu plików (blok, block_s1, block_s2)
    bool loadAtlasFromFiles(const std::vector<std::string>& paths);

    bool loadSingleTile(const std::string& path);

    void create(unsigned width, unsigned height, TileId defaultTile = 0);

    unsigned getWidth()  const { return width_; }
    unsigned getHeight() const { return height_; }
    sf::Vector2u getTileSizePx() const { return tileSizePx_; }

    TileId get(unsigned x, unsigned y) const;
    void   set(unsigned x, unsigned y, TileId id);

    sf::Vector2i worldToCell(sf::Vector2f worldPos) const;
    sf::Vector2f cellToWorld(unsigned x, unsigned y) const;

    bool saveToFile(const std::string& path) const;
    bool loadFromFile(const std::string& path);

private:
    void rebuildVertices();
    void updateCellVertex(unsigned x, unsigned y);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Texture atlas_;
    sf::Vector2u tileSizePx_{32, 32};

    unsigned width_  = 0;
    unsigned height_ = 0;

    std::vector<TileId> tiles_;
    sf::VertexArray vertices_;
};
