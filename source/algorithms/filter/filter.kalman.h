#pragma once
#include "filter.include.h"
#include <chrono>
#include <memory>
#include <mutex>
#include <opencv2/core.hpp>

namespace tianli::algorithms::filter
{
    class KalmanFilter
    {
    public:
        KalmanFilter() {}
        KalmanFilter(int dynamParams, int measureParams, int controlParams = 0, int type = CV_32F) { init(dynamParams, measureParams, controlParams, type); }
        void init(int DP, int MP, int CP = 0, int type = CV_32F) // init(int dynamParams, int measureParams, int controlParams = 0, int type = CV_32F)
        {
            CP = (std::max)(CP, 0);

            statePre = cv::Mat::zeros(DP, 1, type);
            statePost = cv::Mat::zeros(DP, 1, type);
            transitionMatrix = cv::Mat::eye(DP, DP, type);

            processNoiseCov = cv::Mat::eye(DP, DP, type);
            measurementMatrix = cv::Mat::zeros(MP, DP, type);
            measurementNoiseCov = cv::Mat::eye(MP, MP, type);

            errorCovPre = cv::Mat::zeros(DP, DP, type);
            errorCovPost = cv::Mat::zeros(DP, DP, type);
            gain = cv::Mat::zeros(DP, MP, type);

            if (CP > 0)
                controlMatrix = cv::Mat::zeros(DP, CP, type);
            else
                controlMatrix.release();

            temp1.create(DP, DP, type);
            temp2.create(MP, DP, type);
            temp3.create(MP, MP, type);
            temp4.create(MP, DP, type);
            temp5.create(MP, 1, type);
        }

        const cv::Mat& predict(const cv::Mat& control = cv::Mat())
        {

            // update the state: x'(k) = A*x(k)
            statePre = transitionMatrix * statePost;

            if (!control.empty())
                // x'(k) = x'(k) + B*u(k)
                statePre += controlMatrix * control;

            // update error covariance matrices: temp1 = A*P(k)
            temp1 = transitionMatrix * errorCovPost;

            // P'(k) = temp1*At + Q
            gemm(temp1, transitionMatrix, 1, processNoiseCov, 1, errorCovPre, cv::GEMM_2_T);

            // handle the case when there will be no measurement before the next predict.
            statePre.copyTo(statePost);
            errorCovPre.copyTo(errorCovPost);

            return statePre;
        }
        const cv::Mat& correct(const cv::Mat& measurement)
        {
            // temp2 = H*P'(k)
            temp2 = measurementMatrix * errorCovPre;

            // temp3 = temp2*Ht + R
            cv::gemm(temp2, measurementMatrix, 1, measurementNoiseCov, 1, temp3, cv::GEMM_2_T);

            // temp4 = inv(temp3)*temp2 = Kt(k)
            cv::solve(temp3, temp2, temp4, cv::DECOMP_SVD);

            // K(k)
            gain = temp4.t();

            // temp5 = z(k) - H*x'(k)
            temp5 = measurement - measurementMatrix * statePre;

            // x(k) = x'(k) + K(k)*temp5
            statePost = statePre + gain * temp5;

            // P(k) = P'(k) - K(k)*temp2
            errorCovPost = errorCovPre - gain * temp2;

            return statePost;
        }

        cv::Mat statePre;            //!< predicted state (x'(k)): x(k)=A*x(k-1)+B*u(k)
        cv::Mat statePost;           //!< corrected state (x(k)): x(k)=x'(k)+K(k)*(z(k)-H*x'(k))
        cv::Mat transitionMatrix;    //!< state transition matrix (A)
        cv::Mat controlMatrix;       //!< control matrix (B) (not used if there is no control)
        cv::Mat measurementMatrix;   //!< measurement matrix (H)
        cv::Mat processNoiseCov;     //!< process noise covariance matrix (Q)
        cv::Mat measurementNoiseCov; //!< measurement noise covariance matrix (R)
        cv::Mat errorCovPre;         //!< priori error estimate covariance matrix (P'(k)): P'(k)=A*P(k-1)*At + Q)*/
        cv::Mat gain;                //!< Kalman gain matrix (K(k)): K(k)=P'(k)*Ht*inv(H*P'(k)*Ht+R)
        cv::Mat errorCovPost;        //!< posteriori error estimate covariance matrix (P(k)): P(k)=(I-K(k)*H)*P'(k)

        // temporary matrices
        cv::Mat temp1;
        cv::Mat temp2;
        cv::Mat temp3;
        cv::Mat temp4;
        cv::Mat temp5;
    };

    struct position_estimater
    {
        cv::Point2d get_position();
        void set_high_resolution_position(const cv::Point2d& pos, std::chrono::system_clock::time_point time);
        void set_low_resolution_position(const cv::Point2d& pos, std::chrono::system_clock::time_point time);

        struct impl_t;
        std::unique_ptr<impl_t> impl;

        position_estimater();
        ~position_estimater(); // 需要析构函数的声明，因为impl_t是不完整类型
    };

    // position_estimater的实现
    struct position_estimater::impl_t
    {
        KalmanFilter kf;
        std::chrono::system_clock::time_point last_time;
        bool initialized;
        std::mutex mtx;
        double sigma_a; // 加速度噪声标准差

        impl_t() : kf(4, 2, 0), initialized(false), sigma_a(0.5)
        {
            // 初始化测量矩阵 H
            kf.measurementMatrix = (cv::Mat_<double>(2, 4) << 1, 0, 0, 0, 0, 1, 0, 0);
            // 初始转移矩阵 F (dt=0)
            kf.transitionMatrix = cv::Mat::eye(4, 4, CV_64F);
            // 初始过程噪声协方差，之后每次更新
            cv::setIdentity(kf.processNoiseCov, cv::Scalar::all(1e-5));
            // 初始测量噪声协方差，之后根据高低精度设置
            cv::setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1e-1));
            // 初始后验误差协方差
            cv::setIdentity(kf.errorCovPost, cv::Scalar::all(0.1));
        }

        void update(const cv::Point2d& pos, const std::chrono::system_clock::time_point& time, bool is_high_res)
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!initialized)
            {
                kf.statePost = (cv::Mat_<double>(4, 1) << pos.x, pos.y, 0.0, 0.0);
                last_time = time;
                initialized = true;
                return;
            }

            auto dt_duration = time - last_time;
            double dt = std::chrono::duration_cast<std::chrono::duration<double>>(dt_duration).count();
            if (dt <= 0.0)
            {
                return;
            }

            // 更新转移矩阵 F
            cv::Mat& F = kf.transitionMatrix;
            F.at<double>(0, 2) = dt;
            F.at<double>(1, 3) = dt;

            // 更新过程噪声协方差 Q
            double dt2 = dt * dt;
            double dt3 = dt2 * dt;
            double dt4 = dt3 * dt;
            double sigma_a2 = sigma_a * sigma_a;

            cv::Mat Q = (cv::Mat_<double>(4, 4) << dt4 / 4 * sigma_a2, 0, dt3 / 2 * sigma_a2, 0, 0, dt4 / 4 * sigma_a2, 0, dt3 / 2 * sigma_a2, dt3 / 2 * sigma_a2, 0, dt2 * sigma_a2, 0, 0,
                         dt3 / 2 * sigma_a2, 0, dt2 * sigma_a2);
            kf.processNoiseCov = Q;

            // 预测
            kf.predict();

            // 设置测量噪声
            if (is_high_res)
            {
                cv::setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1e-1)); // 高频噪声大
            }
            else
            {
                cv::setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1e-3)); // 低频噪声小
            }

            // 更新
            cv::Mat measurement = (cv::Mat_<double>(2, 1) << pos.x, pos.y);
            kf.correct(measurement);

            last_time = time;
        }

        cv::Point2d get_position()
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!initialized)
            {
                return cv::Point2d(0.0, 0.0);
            }
            return cv::Point2d(kf.statePost.at<double>(0), kf.statePost.at<double>(1));
        }
    };

    position_estimater::position_estimater() : impl(new impl_t()) {}
    position_estimater::~position_estimater() = default;

    cv::Point2d position_estimater::get_position()
    {
        return impl->get_position();
    }

    void position_estimater::set_high_resolution_position(const cv::Point2d& pos, std::chrono::system_clock::time_point time)
    {
        impl->update(pos, time, true);
    }

    void position_estimater::set_low_resolution_position(const cv::Point2d& pos, std::chrono::system_clock::time_point time)
    {
        impl->update(pos, time, false);
    }

    class filter_kalman : public filter
    {
    public:
        filter_kalman(std::shared_ptr<global::logger> logger = nullptr) : filter(logger)
        {
            this->type = filter_type::kalman;

            KF = KalmanFilter(stateNum, measureNum, controlNum);
            state = cv::Mat(stateNum, 1, CV_32F);
            processNoise = cv::Mat(stateNum, 1, CV_32F);
            measurement = cv::Mat::zeros(measureNum, 1, CV_32F); // measurement(x,y)

            randn(state, cv::Scalar::all(0), cv::Scalar::all(0.1)); // 随机生成一个矩阵，期望是0，标准差为0.1;
            // set A
            KF.transitionMatrix = (cv::Mat_<float>(stateNum, stateNum) << 1, 0, 0, 1);
            // set B
            KF.controlMatrix = (cv::Mat_<float>(stateNum, controlNum) << 1, 0, 0, 1);
            // set Q
            setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-5));
            // set H
            KF.measurementMatrix = (cv::Mat_<float>(measureNum, stateNum) << 1, 0, 0, 1);
            // set R
            setIdentity(KF.measurementNoiseCov, cv::Scalar::all(1e-3));

            randn(KF.statePost, cv::Scalar::all(0), cv::Scalar::all(0.1));
        }
        ~filter_kalman() override = default;

        cv::Point2d filterting(const cv::Point2d& pos, const cv::Point2f& u_k) override
        {
            // KF中弃用！
            // 若要调用，相当于调用了一次predict和一次update

            // use u_k to predict
            // make u_k to cv::Mat
            cv::Mat u_k_mat = cv::Mat::zeros(controlNum, 1, CV_32F);
            u_k_mat.at<float>(0, 0) = u_k.x;
            u_k_mat.at<float>(1, 0) = u_k.y;
            cv::Mat prediction = KF.predict(u_k_mat);
            cv::Point2d predictPt = cv::Point2d(prediction.at<float>(0), prediction.at<float>(1));

            // 3.update measurement
            measurement.at<float>(0, 0) = static_cast<float>(pos.x);
            measurement.at<float>(1, 0) = static_cast<float>(pos.y);

            // 4.update
            KF.correct(measurement);

            cv::Point2d resP = cv::Point2d(KF.statePost.at<float>(0), KF.statePost.at<float>(1));
            return resP;
        }

        cv::Point2d re_init_filterting(const cv::Point2d& pos) override
        {
            // set state
            KF.statePost.at<float>(0) = static_cast<float>(pos.x);
            KF.statePost.at<float>(1) = static_cast<float>(pos.y);
            // predict without u_k
            cv::Mat prediction = KF.predict();
            cv::Point2d predictPt = cv::Point2d(prediction.at<float>(0), prediction.at<float>(1));

            // 3.update measurement
            measurement.at<float>(0, 0) = static_cast<float>(pos.x);
            measurement.at<float>(1, 0) = static_cast<float>(pos.y);
            KF.correct(measurement);

            cv::Point2d resP = cv::Point2d(KF.statePost.at<float>(0), KF.statePost.at<float>(1));
            return resP;
        }

        cv::Point2d predict(const cv::Point2f& u_k) override
        {
            // use u_k to predict
            // make u_k to cv::Mat
            cv::Mat u_k_mat = cv::Mat::zeros(controlNum, 1, CV_32F);
            u_k_mat.at<float>(0, 0) = u_k.x;
            u_k_mat.at<float>(1, 0) = u_k.y;
            cv::Mat prediction = KF.predict(u_k_mat);
            cv::Point2d predictPt = cv::Point2d(prediction.at<float>(0), prediction.at<float>(1));
            return predictPt;
        }

        cv::Point2d update(const cv::Point2d& pos) override
        {
            // update measurement
            measurement.at<float>(0, 0) = static_cast<float>(pos.x);
            measurement.at<float>(1, 0) = static_cast<float>(pos.y);
            // update
            KF.correct(measurement);
            cv::Point2d resP = cv::Point2d(KF.statePost.at<float>(0), KF.statePost.at<float>(1));
            return resP;
        }

    private:
        KalmanFilter KF;

        int stateNum = 2;
        int measureNum = 2;
        int controlNum = 2;

        // motion model
        // x(k) = A*x(k-1) + B*u(k) + w(k)
        // w(k) ~ N(0, Q)

        // update model
        // z(k) = H*x(k) + v(k)
        // v(k) ~ N(0, R)

        // Where:
        // x = [x y]'
        // z = [x y]'
        // u = [dx dy]'
        // A = [1 0; 0 1]
        // B = [1 0; 0 1]
        // H = [1 0; 0 1]
        // Q = [1 0; 0 1] * 1e-5
        // R = [1 0; 0 1] * 1e-5
        cv::Mat state;
        cv::Mat processNoise;
        cv::Mat measurement;
    };

} // namespace tianli::algorithms::filter
