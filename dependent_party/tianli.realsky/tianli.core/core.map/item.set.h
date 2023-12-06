#pragma once
#include <map>
#include <regex>
#include <future>
#include <ranges>
#include <memory>
#include <vector>
#include <numeric>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include "map.inface.h"

class KeyPointObject : public ItemInface
{
public:
    KeyPointObject(std::shared_ptr<cv::KeyPoint> keypoint, size_t index) : ItemInface(keypoint->pt), kp(keypoint), index(index) {}
    KeyPointObject(cv::KeyPoint keypoint, size_t index) : ItemInface(keypoint.pt), kp(std::make_shared<cv::KeyPoint>(keypoint)), index(index) {}
    ~KeyPointObject() = default;
    cv::Point2d pos() override { return kp->pt; }
    std::shared_ptr<cv::KeyPoint> kp;
    size_t index = 0;
};

class ItemObject : public ItemInface
{
public:
    ItemObject(const cv::Point2d &pos, const std::string &name, const cv::Mat &image = cv::Mat(), const std::string &description = "")
        : ItemInface(pos), name(name), image(image), description(description) {}
    ~ItemObject() = default;
    std::string name;
    size_t index = 0;
    cv::Mat image;
    std::string description;
};

/// @brief 用来存储物品项集合的四叉树实现类
class MapSet : public ItemSetInface
{
public:
    MapSet() = default;
    ~MapSet() = default;
    MapSet(const cv::Rect2d &rect, const std::vector<std::shared_ptr<ItemInface>> &items)
    {
        root = std::make_shared<Node>();
        root->rect = rect;
        root->center = rect.tl() + cv::Point2d(rect.width / 2.0, rect.height / 2.0);
        for (auto &item : items)
            root->insert(item);
    }

public:
    class Node : public std::enable_shared_from_this<Node>
    {
    public:
        enum class SplitType
        {
            top_left,
            top_right,
            bottom_left,
            bottom_right
        };

    public:
        Node() = default;
        Node(const std::vector<std::shared_ptr<ItemInface>> items)
        {
            for (auto &item : items)
                insert(item);
        }
        Node(std::shared_ptr<Node> parent, SplitType split_type) : parent(parent)
        {
            // set rect and center
            auto split_size = cv::Size2d(parent->rect.width / 2.0, parent->rect.height / 2.0);
            auto split_center = cv::Point2d(parent->rect.width / 4.0, parent->rect.height / 4.0);
            switch (split_type)
            {
            case SplitType::top_left:
                rect = cv::Rect2d(parent->rect.tl(), split_size);
                center = rect.tl() + split_center;
                break;
            case SplitType::top_right:
                rect = cv::Rect2d(parent->rect.tl() + cv::Point2d(parent->rect.width / 2, 0), split_size);
                center = rect.tl() + split_center;
                break;
            case SplitType::bottom_left:
                rect = cv::Rect2d(parent->rect.tl() + cv::Point2d(0, parent->rect.height / 2), split_size);
                center = rect.tl() + split_center;
                break;
            case SplitType::bottom_right:
                rect = cv::Rect2d(parent->rect.tl() + cv::Point2d(parent->rect.width / 2, parent->rect.height / 2), split_size);
                center = rect.tl() + split_center;
                break;
            default:
                throw std::runtime_error("Node unknown split type");
            }
            // split items to childs
            auto copy_items = parent->items;
            for (auto &item : copy_items)
            {
                if (is_intersect(item->pos()))
                    this->items.push_back(item);
                // remove item from parent
                parent->items.remove(item);
            }
            item_set_size = this->items.size();
        }

        ~Node() = default;

    public:
        cv::Rect2d rect;
        cv::Point2d center;

    public:
        std::list<std::shared_ptr<ItemInface>> items;
        size_t item_set_size = 0;
        size_t node_count = 1; // 至少会有root节点

    public:
        size_t node_item_max = 128; // 32;// 512;

    public:
        bool is_leaf() { return childs.empty(); }
        bool is_empty() { return items.empty(); }
        bool is_intersect(const cv::Rect2d &rect) { return (this->rect & rect).area() > 0; }
        bool is_intersect(const cv::Point2d &pos) { return this->rect.contains(pos); }
        /// @brief 获取当前节点的物品数量
        /// @return
        size_t size() { return items.size(); }
        /// @brief 获取当前节点包括递归子节点的物品数量
        /// @return
        size_t sizes() { return item_set_size; }
        /// @brief 获取当前节点的子节点数量
        size_t count() { return childs.size(); }
        /// @brief 获取当前节点包括递归子节点的子节点数量
        /// @return
        size_t counts() { return node_count; }

    public:
        std::vector<std::shared_ptr<Node>> split()
        {
            if (is_leaf() == false)
                return childs;
            auto that = this->shared_from_this();
            top_left = std::make_shared<Node>(that, SplitType::top_left);
            top_right = std::make_shared<Node>(that, SplitType::top_right);
            bottom_left = std::make_shared<Node>(that, SplitType::bottom_left);
            bottom_right = std::make_shared<Node>(that, SplitType::bottom_right);
            childs = {top_left, top_right, bottom_left, bottom_right};
            if (items.empty() == false)
                throw std::runtime_error("Node split error");
            // 递归到root 增加node_count计数
            while (that != nullptr)
            {
                that->node_count = that->node_count + 4;
                that = that->parent.lock();
            }
            return childs;
        }
        bool insert(const std::shared_ptr<ItemInface> &item)
        {
            if (item == nullptr)
                return false;
            // 如果当前节点与物品不相交，直接返回
            if (is_intersect(item->pos()) == false)
                return false;
            item_set_size++;
            // 如果当前节点是叶子节点
            if (is_leaf())
            {
                // 直接插入
                items.push_back(item);
                // 如果当前节点的物品数量超过阈值
                if (items.size() > node_item_max)
                {
                    // 需要将当前节点分裂
                    this->split();
                }
                return true;
            }
            // 如果当前节点不是叶子节点
            // 将物品插入到子节点中
            for (auto &child : childs)
                if (child->insert(item))
                    return true;
            // 如果所有子节点都没有插入成功
            return false;
        }
        std::vector<std::shared_ptr<ItemInface>> find(const cv::Rect2d &rect)
        {
            std::vector<std::shared_ptr<ItemInface>> rect_items;
            // 如果当前节点与范围不相交，直接返回
            if (is_intersect(rect) == false)
                return rect_items;
            // 如果当前节点是叶子节点
            if (is_leaf())
            {
                // 如果当前节点范围完全包含在范围内，直接返回当前节点的物品集合
                if (rect.contains(rect.tl()) && rect.contains(rect.br()))
                {
                    rect_items.insert(rect_items.end(), items.begin(), items.end());
                    return rect_items;
                }
                // 将范围与物品相交的物品插入到结果中
                for (auto &item : items)
                    if (rect.contains(item->pos()))
                        rect_items.push_back(item);
                return rect_items;
            }
            // 如果当前节点不是叶子节点
            // 将范围与子节点相交的子节点的物品插入到结果中
            for (auto &child : childs)
                if (child->is_intersect(rect))
                {
                    auto child_items = child->find(rect);
                    rect_items.insert(rect_items.end(), child_items.begin(), child_items.end());
                }
            return rect_items;
        }
        /// @brief 查找范围内的递归子节点
        /// @param rect 范围
        /// @return std::list<std::shared_ptr<Node>> 子节点集合
        std::list<std::shared_ptr<Node>> find_childs(const cv::Rect2d &rect)
        {
            if (is_intersect(rect) == false)
                return {};
            if (is_leaf() && items.empty() == false)
                return {this->shared_from_this()};
            std::list<std::shared_ptr<Node>> childs;
            for (auto &child : this->childs)
                if (child->is_intersect(rect))
                {
                    auto child_childs = child->find_childs(rect);
                    childs.insert(childs.end(), child_childs.begin(), child_childs.end());
                }
            return childs;
        }

    public:
        std::weak_ptr<Node> parent;
        std::shared_ptr<Node> top_left;
        std::shared_ptr<Node> top_right;
        std::shared_ptr<Node> bottom_left;
        std::shared_ptr<Node> bottom_right;
        std::vector<std::shared_ptr<Node>> childs;
        // = {top_left, top_right, bottom_left, bottom_right};
    };
    std::shared_ptr<Node> root;

public:
    std::vector<std::shared_ptr<ItemInface>> find(const cv::Rect2d &rect) override
    {
        if (root == nullptr)
            return {};
        return root->find(rect);
    }
    std::list<std::shared_ptr<Node>> find_childs(const cv::Rect2d &rect)
    {
        if (root == nullptr)
            return {};
        return root->find_childs(rect);
    }

public:
    void cout(std::shared_ptr<Node> node = nullptr, int depth = 0)
    {
        if (node == nullptr && depth != 0)
            return;
        if (node == nullptr)
            node = root;

        auto space = std::string();
        for (int i = 0; i < depth; i++)
            space += "\t";
        std::cout << space << "node: " << node->rect << std::endl;
        std::cout << space << "node size: " << node->item_set_size << std::endl;
        std::cout << space << "node node count: " << node->counts() << std::endl;
        for (auto &item : node->items)
            std::cout << space << "\titem: " << item->pos() << std::endl;
        for (auto &child : node->childs)
            cout(child, depth + 1);
    }
    void print()
    {
        double scale = 15;
        auto size = cv::Size(static_cast<int>(root->rect.size().width / scale), static_cast<int>(root->rect.size().height / scale));
        cv::Mat img = cv::Mat::zeros(size, CV_8UC3);
        auto pos_offset = cv::Point2d(-root->rect.tl()) / scale;
        int count = 0;
        int max_depth = 0;
        // 遍历树
        std::function<void(std::shared_ptr<Node>, int)> print_node = [&](std::shared_ptr<Node> node, int depth = 0)
        {
            if (node == nullptr)
                return;
            count++;

            print_node(node->top_left, depth + 1);
            print_node(node->top_right, depth + 1);
            print_node(node->bottom_left, depth + 1);
            print_node(node->bottom_right, depth + 1);
            if (node->is_leaf() == false)
                return;

            if (depth > max_depth)
                max_depth = depth;
            // 绘制树
            auto rect = cv::Rect(static_cast<int>(node->rect.x / scale), static_cast<int>(node->rect.y / scale), static_cast<int>(node->rect.width / scale), static_cast<int>(node->rect.height / scale)) + cv::Point(pos_offset);
            cv::rectangle(img, rect, cv::Scalar(255, 255, depth * 8), 1, cv ::LINE_AA);
            cv::circle(img, node->center / scale + pos_offset, 1, cv::Scalar(0, depth * 8, 255), 1, cv::LINE_AA);
            for (auto &item : node->items)
                cv::circle(img, item->pos() / scale + pos_offset, 1, cv::Scalar(0, 255, depth * 8), 1, cv::LINE_AA);
            // cv::imshow("tree", img);
            // cv::waitKey(1);
        };
        print_node(root, 0);

        std::cout << "node count: " << count << std::endl;
        std::cout << "max depth: " << max_depth << std::endl;
        cv::imwrite("tree.png", img);
    }
};
