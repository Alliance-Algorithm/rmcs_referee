#include <memory>

#include <eigen3/Eigen/Eigen>

#include <rclcpp/node.hpp>
#include <rclcpp/publisher.hpp>

#include <geometry_msgs/msg/pose_stamped.hpp>
#include <rmcs_msgs/msg/game_status.hpp>
#include <std_msgs/msg/detail/int32__struct.hpp>
#include <std_msgs/msg/float32_multi_array.hpp>
#include <std_msgs/msg/int32.hpp>
#include <std_msgs/msg/int8.hpp>

#include "command/field.hpp"
#include "command/interaction/header.hpp"
#include "rmcs_executor/component.hpp"
#include "rmcs_msgs/robot_id.hpp"
#include "status/field.hpp"

namespace rmcs_referee::command::interaction {

class Communicate : public rmcs_executor::Component, public rclcpp::Node {

public:
  Communicate() : rclcpp::Node(get_component_name()) {

    register_output("/referee/command/interaction/communicate",
                    communicate_field_);

    register_input("/referee/enemies/hero/position", enemies_hero_sentry_link_,
                   false);
    register_input("/referee/enemies/engineer/position",
                   enemies_engineer_sentry_link_, false);
    register_input("/referee/enemies/infantry_iii/position",
                   enemies_infantry_iii_sentry_link_, false);
    register_input("/referee/enemies/infantry_iv/position",
                   enemies_infantry_iv_sentry_link_, false);
    register_input("/referee/enemies/infantry_v/position",
                   enemies_infantry_v_sentry_link_, false);
    register_input("/referee/enemies/sentry/position",
                   enemies_sentry_sentry_link_, false);

    register_input("/referee/friends/hero/position", friends_hero_origin_link_,
                   false);
    register_input("/referee/friends/engineer/position",
                   friends_engineer_origin_link_, false);
    register_input("/referee/friends/infantry_iii/position",
                   friends_infantry_iii_origin_link_, false);
    register_input("/referee/friends/infantry_iv/position",
                   friends_infantry_iv_origin_link_, false);
    register_input("/referee/friends/infantry_v/position",
                   friends_infantry_v_origin_link_, false);
    register_input("/referee/friends/sentry/position",
                   friends_sentry_origin_link_, false);

    register_input("/referee/id", robot_id_, false);
    register_input("/referee/robots/hp", robots_hp_, false);
    register_input("/referee/shooter/bullet_allowance", bullet_, false);

    position_publisher_ = create_publisher<std_msgs::msg::Float32MultiArray>(
        "/unit_info/enemy/position", 10);
    hp_publisher_ = create_publisher<std_msgs::msg::Float32MultiArray>(
        "/unit_info/enemy/hp", 10);
    bullet_count_publisher_ =
        create_publisher<std_msgs::msg::Int32>("/referee/sentry/bullet", 10);

    pose_subscription_ = create_subscription<geometry_msgs::msg::PoseStamped>(
        "/rmcs_navigation/pose", 10,
        [this](const std::unique_ptr<geometry_msgs::msg::PoseStamped> &msg) {
          pose_subscription_callback(msg);
        });
  }

  void update() override {
    static int frame_count = 0;
    if (++frame_count % 100)
      return;

    auto header = Header{0x222, 7, 9};

    auto status = rmcs_msgs::msg::GameStatus{};

    if (*robot_id_ != rmcs_msgs::RobotId::UNKNOWN) {
      if (robot_id_->color() == rmcs_msgs::RobotColor::RED) {
        translation_origin_to_init_ = Eigen::Translation3d{6.5, 7.5, 0};
        rotation_origin_to_init_ = Eigen::Quaterniond::Identity();

        // status.friends_base_hp = robots_hp_->red_base;
        // status.friends_outpost_hp = robots_hp_->red_outpost;
        // status.friends_hero.hp = robots_hp_->red_1;
        // status.friends_engineer.hp = robots_hp_->red_2;
        // status.friends_infantry_iii.hp = robots_hp_->red_3;
        // status.friends_infantry_iv.hp = robots_hp_->red_4;
        // status.friends_infantry_v.hp = robots_hp_->red_5;
        // status.friends_sentry.hp = robots_hp_->red_7;

        status.enemies_base_hp = robots_hp_->blue_base;
        status.enemies_outpost_hp = robots_hp_->blue_outpost;
        status.enemies_hero.hp = robots_hp_->blue_1;
        status.enemies_engineer.hp = robots_hp_->blue_2;
        status.enemies_infantry_iii.hp = robots_hp_->blue_3;
        status.enemies_infantry_iv.hp = robots_hp_->blue_4;
        status.enemies_infantry_v.hp = robots_hp_->blue_5;
        status.enemies_sentry.hp = robots_hp_->blue_7;

        header.sender_id = 7;
        header.receiver_id = 9;
      } else {
        translation_origin_to_init_ = Eigen::Translation3d{21.5, 7.5, 0};
        rotation_origin_to_init_ = Eigen::Quaterniond{
            Eigen::AngleAxisd{std::numbers::pi, Eigen::Vector3d::UnitZ()}};

        // status.friends_base_hp = robots_hp_->blue_base;
        // status.friends_outpost_hp = robots_hp_->blue_outpost;
        // status.friends_hero.hp = robots_hp_->blue_1;
        // status.friends_engineer.hp = robots_hp_->blue_2;
        // status.friends_infantry_iii.hp = robots_hp_->blue_3;
        // status.friends_infantry_iv.hp = robots_hp_->blue_4;
        // status.friends_infantry_v.hp = robots_hp_->blue_5;
        // status.friends_sentry.hp = robots_hp_->blue_7;

        status.enemies_base_hp = robots_hp_->red_base;
        status.enemies_outpost_hp = robots_hp_->red_outpost;
        status.enemies_hero.hp = robots_hp_->red_1;
        status.enemies_engineer.hp = robots_hp_->red_2;
        status.enemies_infantry_iii.hp = robots_hp_->red_3;
        status.enemies_infantry_iv.hp = robots_hp_->red_4;
        status.enemies_infantry_v.hp = robots_hp_->red_5;
        status.enemies_sentry.hp = robots_hp_->red_7;

        header.sender_id = 107;
        header.receiver_id = 109;
      }
    }

    auto hero_origin_link = transform(*enemies_hero_sentry_link_);
    auto engineer_origin_link_ = transform(*enemies_engineer_sentry_link_);
    auto infantry_iii_origin_link_ =
        transform(*enemies_infantry_iii_sentry_link_);
    auto infantry_iv_origin_link_ =
        transform(*enemies_infantry_iv_sentry_link_);
    auto infantry_v_origin_link_ = transform(*enemies_infantry_v_sentry_link_);
    auto sentry_origin_link_ = transform(*enemies_sentry_sentry_link_);

    // std_msgs::msg::Float32MultiArray msg;
    // msg.data.push_back(static_cast<float>(hero_origin_link.x()));
    // msg.data.push_back(static_cast<float>(engineer_origin_link_.x()));
    // msg.data.push_back(static_cast<float>(infantry_iii_origin_link_.x()));
    // msg.data.push_back(static_cast<float>(infantry_iv_origin_link_.x()));
    // msg.data.push_back(static_cast<float>(infantry_v_origin_link_.x()));
    // msg.data.push_back(static_cast<float>(hero_origin_link.y()));
    // msg.data.push_back(static_cast<float>(engineer_origin_link_.y()));
    // msg.data.push_back(static_cast<float>(infantry_iii_origin_link_.y()));
    // msg.data.push_back(static_cast<float>(infantry_iv_origin_link_.y()));
    // msg.data.push_back(static_cast<float>(infantry_v_origin_link_.y()));
    // position_publisher_->publish(msg);

    // status.friends_hero.pose.x = friends_hero_origin_link_->x();
    // status.friends_hero.pose.y = friends_hero_origin_link_->y();
    // status.friends_engineer.pose.x =
    // friends_engineer_origin_link_->x();
    // status.friends_engineer.pose.y =
    // friends_engineer_origin_link_->y();
    // status.friends_infantry_iii.pose.x =
    // friends_infantry_iii_origin_link_->x();
    // status.friends_infantry_iii.pose.y =
    // friends_infantry_iii_origin_link_->y();
    // status.friends_infantry_iv.pose.x =
    // friends_infantry_iv_origin_link_->x();
    // status.friends_infantry_iv.pose.y =
    // friends_infantry_iv_origin_link_->y();
    // status.friends_infantry_v.pose.x =
    // friends_infantry_v_origin_link_->x();
    // status.friends_infantry_v.pose.y =
    // friends_infantry_v_origin_link_->y();
    // status.friends_sentry.pose.x = friends_sentry_origin_link_->x();
    // status.friends_sentry.pose.y = friends_sentry_origin_link_->y();

    // status.bullet = *bullet_;

    std_msgs::msg::Int32 msg_bullet;
    msg_bullet.data = *bullet_;
    bullet_count_publisher_->publish(msg_bullet);

    auto data = SensorData{};

    data.position[0].x = static_cast<float>(hero_origin_link.x());
    data.position[0].y = static_cast<float>(hero_origin_link.y());
    data.position[1].x = static_cast<float>(engineer_origin_link_.x());
    data.position[1].y = static_cast<float>(engineer_origin_link_.y());
    data.position[2].x = static_cast<float>(infantry_iii_origin_link_.x());
    data.position[2].y = static_cast<float>(infantry_iii_origin_link_.y());
    data.position[3].x = static_cast<float>(infantry_iv_origin_link_.x());
    data.position[3].y = static_cast<float>(infantry_iv_origin_link_.y());
    data.position[4].x = static_cast<float>(infantry_v_origin_link_.x());
    data.position[4].y = static_cast<float>(infantry_v_origin_link_.y());
    data.position[5].x = static_cast<float>(sentry_origin_link_.x());
    data.position[5].y = static_cast<float>(sentry_origin_link_.y());

    communicate_field_;
  }

private:
  struct __attribute__((packed)) SensorData {
    struct {
      float x;
      float y;
    } position[6];
  };

private:
  OutputInterface<Field> communicate_field_;

  InputInterface<Eigen::Vector2d> enemies_hero_sentry_link_;
  InputInterface<Eigen::Vector2d> enemies_engineer_sentry_link_;
  InputInterface<Eigen::Vector2d> enemies_infantry_iii_sentry_link_;
  InputInterface<Eigen::Vector2d> enemies_infantry_iv_sentry_link_;
  InputInterface<Eigen::Vector2d> enemies_infantry_v_sentry_link_;
  InputInterface<Eigen::Vector2d> enemies_sentry_sentry_link_;

  InputInterface<Eigen::Vector2d> friends_hero_origin_link_;
  InputInterface<Eigen::Vector2d> friends_engineer_origin_link_;
  InputInterface<Eigen::Vector2d> friends_infantry_iii_origin_link_;
  InputInterface<Eigen::Vector2d> friends_infantry_iv_origin_link_;
  InputInterface<Eigen::Vector2d> friends_infantry_v_origin_link_;
  InputInterface<Eigen::Vector2d> friends_sentry_origin_link_;

  InputInterface<rmcs_msgs::RobotId> robot_id_;
  InputInterface<status::GameRobotHp> robots_hp_;
  InputInterface<uint16_t> bullet_;

  rclcpp::Publisher<std_msgs::msg::Float32MultiArray>::SharedPtr
      position_publisher_;
  rclcpp::Publisher<std_msgs::msg::Float32MultiArray>::SharedPtr hp_publisher_;
  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr bullet_count_publisher_;

  rclcpp::Publisher<std_msgs::msg::Int8>::SharedPtr founded_publisher_;

  std::shared_ptr<rclcpp::Subscription<geometry_msgs::msg::PoseStamped>>
      pose_subscription_;

  Eigen::Translation3d translation_init_to_sentry_{};
  Eigen::Quaterniond rotation_init_to_sentry_{};

  Eigen::Translation3d translation_origin_to_init_{};
  Eigen::Quaterniond rotation_origin_to_init_{};

private:
  void pose_subscription_callback(
      const std::unique_ptr<geometry_msgs::msg::PoseStamped> &msg) {
    translation_init_to_sentry_.x() = msg->pose.position.x;
    translation_init_to_sentry_.y() = msg->pose.position.y;
    translation_init_to_sentry_.z() = msg->pose.position.z;

    rotation_init_to_sentry_.w() = msg->pose.orientation.w;
    rotation_init_to_sentry_.x() = msg->pose.orientation.x;
    rotation_init_to_sentry_.y() = msg->pose.orientation.y;
    rotation_init_to_sentry_.z() = msg->pose.orientation.z;
  }

  Eigen::Vector2d transform(const Eigen::Vector2d &pose) {
    static Eigen::Vector3d pose3d{Eigen::Vector3d::Identity()};
    pose3d = translation_origin_to_init_ * rotation_origin_to_init_ *
             translation_init_to_sentry_ * rotation_init_to_sentry_ *
             Eigen::Vector3d{pose.x(), pose.y(), 0};
    return {pose3d.x(), pose3d.y()};
  }
};

} // namespace rmcs_referee::command::interaction

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(rmcs_referee::command::interaction::Communicate,
                       rmcs_executor::Component)