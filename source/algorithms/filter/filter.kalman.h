#pragma once
#include <opencv2/opencv.hpp>
#include "filter.include.h"

namespace tianli::algorithms::filter
{
    class KalmanFilter
    {
    public:
        KalmanFilter() {}
        KalmanFilter(int dynamParams, int measureParams, int controlParams = 0, int type = CV_32F)
        {
            init(dynamParams, measureParams, controlParams, type);
        }
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

        const cv::Mat &predict(const cv::Mat &control = cv::Mat())
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
        const cv::Mat &correct(const cv::Mat &measurement)
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
            KF.transitionMatrix = (cv::Mat_<float>(stateNum, stateNum) << 1, 0,
                                   0, 1);
            // set B
            KF.controlMatrix = (cv::Mat_<float>(stateNum, controlNum) << 1, 0,
                                0, 1);
            // set Q
            setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-5));
            // set H
            KF.measurementMatrix = (cv::Mat_<float>(measureNum, stateNum) << 1, 0,
                                    0, 1);
            // set R
            setIdentity(KF.measurementNoiseCov, cv::Scalar::all(1e-3));

            randn(KF.statePost, cv::Scalar::all(0), cv::Scalar::all(0.1));
        }
        ~filter_kalman() override = default;

        cv::Point2d filterting(const cv::Point2d &pos, const cv::Point2f &u_k) override
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

        cv::Point2d re_init_filterting(const cv::Point2d &pos) override
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

        cv::Point2d predict(const cv::Point2f &u_k) override
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

        cv::Point2d update(const cv::Point2d &pos) override
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
