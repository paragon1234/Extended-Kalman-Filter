#include "kalman_filter.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;
const double PI = 3.1415926;

// Please note that the Eigen library does not initialize 
// VectorXd or MatrixXd objects with zeros upon creation.

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
  TODO:
    * predict the state
  */
	x_ = F_ * x_;
	P_ = F_ * P_ * F_.transpose() + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
	// Kalman filter update step. Equations from the lectures
	VectorXd y = z - H_ * x_; // error calculation
	UpdateKF(y);
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
	/**
	TODO:
	* update the state by using Extended Kalman Filter equations
	*/
	// Recalculate x object state to rho, theta, rho_dot coordinates
        //if( fabs(x_(0)) < 0.0001 && fabs(x_(1)) < 0.0001)
        //{
        //    x_(0) = 0.0001;
        //    x_(1) = 0.0001;
        //}
        //else if( fabs(x_(0)) < 0.0001)
        //    x_(0) = 0.0001;
	double rho = sqrt(x_(0)*x_(0) + x_(1)*x_(1));
	double theta = atan2(x_(1) , x_(0));
	if (fabs(x_(0)) < 0.0001)
            theta = 0;
	double rho_dot = 0;

	if (fabs(rho) < 0.0001)
        {
                rho_dot = 0;
		rho = 0;
        }
        else
	    rho_dot = (x_(0)*x_(2) + x_(1)*x_(3)) / rho;
	VectorXd h = VectorXd(3); // h(x_)
	h << rho, theta, rho_dot;

	VectorXd y = z - h;
	while (y(1) > PI) {
		y(1) -= 2*PI;
	}
	while (y(1) < -PI) {
		y(1) += 2*PI;
	}
	// Calculations are essentially the same to the Update function
	UpdateKF(y);
}

void KalmanFilter::UpdateKF(const VectorXd &y) {
  /**
  TODO:
    * update the state by using Kalman Filter equations
  */
	MatrixXd S = H_ * P_ * H_.transpose() + R_;
	MatrixXd PHt = P_ * H_.transpose();
	MatrixXd K = PHt * S.inverse();

	//new estimate
	x_ = x_ + (K * y);
	long x_size = x_.size();
	MatrixXd I = MatrixXd::Identity(x_size, x_size);
	P_ = (I - K * H_) * P_;
}
