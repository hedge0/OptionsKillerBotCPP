#ifndef INTERPOLATIONS_H
#define INTERPOLATIONS_H

#include <config.hpp>
#include <Eigen/Dense>
#include <functional>
#include <iostream>
#include <minimize.hpp>
#include <rbf.hpp>

class interpolations {
  public:
	/**
	* @brief Objective function for optimization; computes the weighted sum of squared residuals.
	*
	* @param params Parameter vector for the model.
	* @param k Log-moneyness vector.
	* @param y_mid Mid values of the dependent variable.
	* @param y_bid Bid values of the dependent variable.
	* @param y_ask Ask values of the dependent variable.
	* @return double The weighted sum of squared residuals.
	*/
	OPTS_BOT_ALWAYS_INLINE static double objective_function(const Eigen::VectorXd& params, const Eigen::VectorXd& k, const Eigen::VectorXd& y_mid, const Eigen::VectorXd& y_bid,
		const Eigen::VectorXd& y_ask) {
		Eigen::VectorXd spread			  = y_ask - y_bid;
		double epsilon					  = 1e-8;
		Eigen::ArrayXd weights			  = 1.0 / (spread.array() + epsilon);
		Eigen::VectorXd model_values	  = interpolations::rfv_model(k, params);
		Eigen::VectorXd residuals		  = model_values - y_mid;
		Eigen::ArrayXd weighted_residuals = weights * residuals.array().square();

		return weighted_residuals.sum();
	}

	/**
	* @brief Creates a radial basis function (RBF) model based on the given data.
	*
	* @param k Input vector representing the independent variable.
	* @param y Output vector representing the dependent variable.
	* @param epsilon Shape parameter for the RBF. If non-positive, it will be computed automatically.
	* @return A function that takes an Eigen::VectorXd and returns the interpolated Eigen::VectorXd.
	*/
	OPTS_BOT_ALWAYS_INLINE static std::function<Eigen::VectorXd(const Eigen::VectorXd&)> rbf_model(const Eigen::VectorXd& k, const Eigen::VectorXd& y, double epsilon) {
		if (epsilon <= 0) {
			Eigen::VectorXd k_sorted = k;
			std::sort(k_sorted.data(), k_sorted.data() + k_sorted.size());
			epsilon = (k_sorted.tail(1)[0] - k_sorted.head(1)[0]) / (k_sorted.size() - 1);
		}

		RBFInterpolator rbf(k, y, epsilon);

		return [rbf](const Eigen::VectorXd& inputs) mutable -> Eigen::VectorXd {
			return rbf.interpolate(inputs);
		};
	}

	/**
	* @brief Computes the Rational Function Volatility (RFV) model values for the given parameters.
	*
	* @param k Log-moneyness vector.
	* @param params Parameter vector [a, b, c, d, e] for the RFV model.
	* @return Eigen::VectorXd The computed RFV model values.
	*/
	OPTS_BOT_ALWAYS_INLINE static Eigen::VectorXd rfv_model(const Eigen::VectorXd& k, const Eigen::VectorXd& params) {
		assert(params.size() == 5 && "Params vector must have 5 elements [a, b, c, d, e]");

		double a = params(0);
		double b = params(1);
		double c = params(2);
		double d = params(3);
		double e = params(4);

		Eigen::ArrayXd k_array = k.array();

		Eigen::ArrayXd numerator   = a + b * k_array + c * k_array.square();
		Eigen::ArrayXd denominator = 1.0 + d * k_array + e * k_array.square();

		return numerator / denominator;
	}

	/**
	* @brief Fits the RFV model to the data by minimizing the objective function.
	*
	* @param x Independent variable data.
	* @param y_mid Mid values of the dependent variable.
	* @param y_bid Bid values of the dependent variable.
	* @param y_ask Ask values of the dependent variable.
	* @return Eigen::VectorXd The optimized parameters vector.
	*/
	OPTS_BOT_ALWAYS_INLINE static Eigen::VectorXd fit_model(const Eigen::VectorXd& x, const Eigen::VectorXd& y_mid, const Eigen::VectorXd& y_bid, const Eigen::VectorXd& y_ask) {
		Eigen::VectorXd k = x.array().log();

		Eigen::VectorXd initial_guess(5);
		initial_guess << 0.2, 0.3, 0.1, 0.2, 0.1;

		std::vector<std::pair<double, double>> bounds(5, std::make_pair(-std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()));

		auto func_grad = [&k, &y_mid, &y_bid, &y_ask](const Eigen::VectorXd& params, Eigen::VectorXd& grad) -> double {
			double f = objective_function(params, k, y_mid, y_bid, y_ask);

			double epsilon		  = 1e-8;
			Eigen::Index n_params = params.size();
			grad				  = Eigen::VectorXd(n_params);
			for (Eigen::Index i = 0; i < n_params; ++i) {
				Eigen::VectorXd params_eps = params;
				params_eps(i) += epsilon;
				double f_eps = objective_function(params_eps, k, y_mid, y_bid, y_ask);
				grad(i)		 = (f_eps - f) / epsilon;
			}
			return f;
		};

		MinimizeResult result{ func_grad, initial_guess, bounds };

		if (result.status != 0) {
			std::cerr << "Optimization failed: " << result.message << std::endl;
		}

		return result.x;
	}
};

#endif
