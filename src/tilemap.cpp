#include "tilemap.hpp"
#include <fstream>
#include <algorithm>

static inline std::size_t idx(unsigned x, unsigned y, unsigned w) {
    return static_cast<std::size_t>(y) * w + x;
}

bool TileMap::loadAtlas(const std::string& atlasPath, sf::Vector2u tileSizePx) {
    tileSizePx_ = tileSizePx;
    return atlas_.loadFromFile(atlasPath);
}

bool TileMap::loadSingleTile(const std::string& path) {
    if (!atlas_.loadFromFile(path)) return false;
    tileSizePx_ = atlas_.getSize(); // 1 kafel = cały obraz
    return tileSizePx_.x > 0 && tileSizePx_.y > 0;
}

void TileMap::create(unsigned width, unsigned height, TileId defaultTile) {
    width_ = width;
    height_ = height;
    tiles_.assign(static_cast<std::size_t>(width_) * height_, defaultTile);

    vertices_.setPrimitiveType(sf::Quads);
    vertices_.resize(static_cast<std::size_t>(width_) * height_ * 4);

    rebuildVertices();
}

TileMap::TileId TileMap::get(unsigned x, unsigned y) const {
    if (x >= width_ || y >= height_) return 0;
    return tiles_[idx(x, y, width_)];
}

void TileMap::set(unsigned x, unsigned y, TileId id) {
    if (x >= width_ || y >= height_) return;
    tiles_[idx(x, y, width_)] = id;
    updateCellVertex(x, y);
}

sf::Vector2i TileMap::worldToCell(sf::Vector2f worldPos) const {
    worldPos = getInverseTransform().transformPoint(worldPos);
    return {
        static_cast<int>(worldPos.x / static_cast<float>(tileSizePx_.x)),
        static_cast<int>(worldPos.y / static_cast<float>(tileSizePx_.y))
    };
}

sf::Vector2f TileMap::cellToWorld(unsigned x, unsigned y) const {
    sf::Vector2f local(
        x * static_cast<float>(tileSizePx_.x),
        y * static_cast<float>(tileSizePx_.y)
    );
    return getTransform().transformPoint(local);
}

void TileMap::rebuildVertices() {
    for (unsigned y = 0; y < height_; ++y)
        for (unsigned x = 0; x < width_; ++x)
            updateCellVertex(x, y);
}

void TileMap::updateCellVertex(unsigned x, unsigned y) {
    const std::size_t quadIndex = idx(x, y, width_) * 4;
    sf::Vertex* quad = &vertices_[quadIndex];

    const float px = x * static_cast<float>(tileSizePx_.x);
    const float py = y * static_cast<float>(tileSizePx_.y);

    quad[0].position = {px, py};
    quad[1].position = {px + tileSizePx_.x, py};
    quad[2].position = {px + tileSizePx_.x, py + tileSizePx_.y};
    quad[3].position = {px, py + tileSizePx_.y};

    TileId id = get(x, y);

    if (id == 0) {
        for (int i = 0; i < 4; ++i) {
            quad[i].texCoords = {0.f, 0.f};
            quad[i].color.a = 0;
        }
        return;
    }

    for (int i = 0; i < 4; ++i) quad[i].color.a = 255;

    if (atlas_.getSize().x < tileSizePx_.x || atlas_.getSize().y < tileSizePx_.y) {
        const float u0 = 0.f, v0 = 0.f;
        const float u1 = static_cast<float>(atlas_.getSize().x);
        const float v1 = static_cast<float>(atlas_.getSize().y);
        quad[0].texCoords = {u0, v0};
        quad[1].texCoords = {u1, v0};
        quad[2].texCoords = {u1, v1};
        quad[3].texCoords = {u0, v1};
        return;
    }

    const unsigned tilesPerRow = std::max(1u, atlas_.getSize().x / tileSizePx_.x);
    const unsigned tu = (id - 1) % tilesPerRow;
    const unsigned tv = (id - 1) / tilesPerRow;

    const float u0 = tu * static_cast<float>(tileSizePx_.x);
    const float v0 = tv * static_cast<float>(tileSizePx_.y);
    const float u1 = u0 + tileSizePx_.x;
    const float v1 = v0 + tileSizePx_.y;

    quad[0].texCoords = {u0, v0};
    quad[1].texCoords = {u1, v0};
    quad[2].texCoords = {u1, v1};
    quad[3].texCoords = {u0, v1};
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &atlas_;
    target.draw(vertices_, states);
}

bool TileMap::saveToFile(const std::string& path) const {
    std::ofstream f(path);
    if (!f) return false;

    f << width_ << " " << height_ << "\n";
    for (unsigned y = 0; y < height_; ++y) {
        for (unsigned x = 0; x < width_; ++x) {
            f << get(x, y) << (x + 1 == width_ ? '\n' : ' ');
        }
    }
    return true;
}

bool TileMap::loadFromFile(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;

    unsigned w, h;
    f >> w >> h;
    create(w, h, 0);

    for (unsigned y = 0; y < h; ++y) {
        for (unsigned x = 0; x < w; ++x) {
            TileId id;
            f >> id;
            set(x, y, id);
        }
    }
    return true;
}

bool TileMap::loadAtlasFromFiles(const std::vector<std::string>& paths) {
    if (paths.empty()) return false;

    std::vector<sf::Image> imgs;
    imgs.reserve(paths.size());

    for (const auto& p : paths) {
        sf::Image img;
        if (!img.loadFromFile(p)) return false;
        imgs.push_back(std::move(img));
    }

    const auto size0 = imgs[0].getSize();
    if (size0.x == 0 || size0.y == 0) return false;

    for (const auto& img : imgs) {
        if (img.getSize() != size0) return false;
    }

    sf::Image atlasImg;
    atlasImg.create(size0.x * static_cast<unsigned>(imgs.size()), size0.y, sf::Color::Transparent);

    for (std::size_t i = 0; i < imgs.size(); ++i) {
        atlasImg.copy(imgs[i], static_cast<unsigned>(i) * size0.x, 0u);
    }

    if (!atlas_.loadFromImage(atlasImg)) return false;

    tileSizePx_ = size0; 
    return true;
}
