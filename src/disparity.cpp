#include "disparity.h"

void Evaluate(Mat standard, Mat myMap) {
	double sum = standard.rows * standard.cols;
	int count = 0;
	int ans = 0;
	for (int i = 0; i < standard.rows; i ++) {
		for (int j = 0; j < standard.cols; j ++) {
			ans = standard.ptr<uchar>(i)[j] - myMap.ptr<uchar>(i)[j];
			//与原图灰度相差大于1可认为是bad pixels，因为增强对比度，所以disparity maps都乘以3显示
			if (ans > 3 || ans < -3) count ++;
		}
	}
	double result = (count + 0.0)/sum;
	cout <<setiosflags(ios::fixed);
	cout <<setprecision(2) << result * 100 << '%' << endl;
}

//type == 1, 2, 3 分别对应取均值、调用opencv的API、运用转换公式
Mat turnIntoGray(Mat input, int type) {
	
	if (input.channels() == 1)return input;

	int row = input.rows;
	int col = input.cols;
	Mat output(row, col, CV_8UC1);

	if (type == 1 || type == 3) {
		for (int i = 0; i < row; i ++) {
			for (int j = 0; j < col; j ++) {
				double temp1 = input.at<Vec3b>(i, j)[0]; //B
				double temp2 = input.at<Vec3b>(i, j)[1]; //G
				double temp3 = input.at<Vec3b>(i, j)[2]; //R
				if (type == 1)output.ptr<uchar>(i)[j] = static_cast<uchar>((temp1 + temp2 + temp3) / 3);
				else output.ptr<uchar>(i)[j] = static_cast<uchar>(temp3 * 0.299 + temp2 * 0.587 + temp1 * 0.114);
			}
		}
	}
	else if (type == 2) {
		cvtColor(input, output, CV_BGR2GRAY);
	}	
	return output;
}

Mat SSD(Mat Left, Mat Right, bool LeftOrRight, int size) {
	int row = Left.rows;
	int col = Left.cols;
	Mat output(row, col, CV_8UC1);
	vector<pair<double, int> > v;
	int size_ = size / 2;
	for (int i = 0; i < row; i ++) {
		for (int j = 0; j < col; j ++) {
			v.clear();
			for (int d = 0; d <= 79; d ++) {
				
				if (j - d < 0 && LeftOrRight) break;
				else if (j + d >= col && (!LeftOrRight))break;

				double temp = 0;
				//因为是自适应的patch大小，所以需要计算窗口内有多少像素，以消除因为窗口大小不一致导致最终结果产生差错
				int count = 0; 
				if (!LeftOrRight)d = -d;
				for (int x = i - size_; x <= i + size_; x ++) {
					for (int y = j - d - size_; y <= j - d + size_; y ++) {
						if (x < 0 || y < 0 || x >= row || y >= col || y + d < 0 || y + d >= col)continue;
						else {
							double temp1;
							double temp2;
							if (LeftOrRight) {
								temp1 = static_cast<double>(Right.ptr<uchar>(x)[y]);
								temp2 = static_cast<double>(Left.ptr<uchar>(x)[y + d]);
							}
							else {
								temp1 = static_cast<double>(Right.ptr<uchar>(x)[y + d]);
								temp2 = static_cast<double>(Left.ptr<uchar>(x)[y]);
							}
							count ++;
							temp += pow(temp1 - temp2, 2);
						}
					}
				}
				if (!LeftOrRight)d = -d;
				v.push_back(make_pair(temp/count, d));
			}

			double min = 2000000000;
			int index = 0;
			for (int k = 0; k < v.size(); k ++) {
				if (v[k].first - min < 0) {
					min = v[k].first;
					index = v[k].second;
				}
			}
			output.ptr<uchar>(i)[j] = static_cast<uchar>(index * 3);
		}
	}
	return output;
}

Mat NCC(Mat Left, Mat Right, bool LeftOrRight, int size) {
	int row = Left.rows;
	int col = Left.cols;
	Mat output(row, col, CV_8UC1);
	vector<pair<double, int> > v;
	int size_ = size / 2;
	for (int i = 0; i < row; i ++) {
		for (int j = 0; j < col; j ++) {
			v.clear();
			for (int d = 0; d <= 79; d ++) {
				if (j - d < 0 && LeftOrRight) break;
				else if (j + d >= col && (!LeftOrRight)) break;
				double tempZ = 0;
				double tempM = 0;
				if (!LeftOrRight)d = -d;
				for (int x = i - size_; x <= i + size_; x ++) {
					for (int y = j - d - size_; y <= j - d + size_; y ++) {
						if (x < 0 || y < 0 || x >= row || y >= col || y + d < 0 || y + d >= col)continue;
						double temp1;
						double temp2;
						if (LeftOrRight) {
							temp1 = static_cast<double>(Right.ptr<uchar>(x)[y]);
							temp2 = static_cast<double>(Left.ptr<uchar>(x)[y + d]);
						}
						else {
							temp1 = static_cast<double>(Right.ptr<uchar>(x)[y + d]);
							temp2 = static_cast<double>(Left.ptr<uchar>(x)[y]);
						}
						tempZ += temp1 * temp2;
					}
				}
				double temp1 = 0;
				double temp2 = 0;
				for (int x = i - size_; x <= i + size_; x ++) {
					for (int y = j - d - size_; y <= j - d + size_; y ++) {
						if (x < 0 || y < 0 || x >= row || y >= col || y + d < 0 || y + d >= col)continue;
						if (LeftOrRight) {
							temp1 += pow(static_cast<double>(Left.ptr<uchar>(x)[y + d]), 2);
							temp2 += pow(static_cast<double>(Right.ptr<uchar>(x)[y]), 2);
						}
						else {
							temp1 += pow(static_cast<double>(Left.ptr<uchar>(x)[y]), 2);
							temp2 += pow(static_cast<double>(Right.ptr<uchar>(x)[y + d]), 2);
						}
					}
				}
				tempM = sqrt(temp1 * temp2);
				double result = tempZ / tempM;
				if (!LeftOrRight)d = -d;
				v.push_back(make_pair(result, d));
			}
			double max = 0;
			int index = 0;
			for (int k = 0; k < v.size(); k ++) {
				if (v[k].first - max > 0) {
					max = v[k].first;
					index = v[k].second;
				}
			}
			output.ptr<uchar>(i)[j] = static_cast<uchar>(index * 3);
		}
	}
	return output;
}

Mat addIntensity(Mat original, int add) {
	int row = original.rows;
	int col = original.cols;
	Mat output(row, col, CV_8UC1);
	for (int i = 0; i < row; i ++) {
		for (int j = 0; j < col; j ++) {
			output.ptr<uchar>(i)[j] = static_cast<uchar>(original.ptr<uchar>(i)[j] + add);
		}
	}
	return output;
}

Mat ASW(Mat Left, Mat Right, bool LeftOrRight, int size, int rc, int rp) {
	int row = Left.rows;
	int col = Left.cols;
	vector<pair<double, int> > v;
	int size_ = size / 2;
	Mat output(row, col, CV_8UC1);

	//change into Lab mode
	Mat LeftLab;
	cvtColor(Left, LeftLab, CV_BGR2Lab); 
	Mat RightLab;
	cvtColor(Right, RightLab, CV_BGR2Lab);

	
	for (int i = 0; i < row; i ++) {
		for (int j = 0; j < col; j ++) {
			v.clear();
			for (int d = 0; d <= 79; d ++) {
				if (j - d < 0 && LeftOrRight) break;
				else if (j + d >= col && (!LeftOrRight)) break;

				double tempZ = 0;
				double tempM = 0;

				if (!LeftOrRight)d = -d;
				for (int x = i - size_; x <= i + size_; x ++) {
					for (int y = j - d - size_; y <= j - d + size_; y ++) {
						if (x < 0 || y < 0 || x >= row || y >= col || y + d < 0 || y + d >= col)continue;
						double Cpq = 0;
						double Cp_q_ = 0;
						double Gpq = 0;
						double Gp_q_ = 0;
						double Wpq = 0;
						double Wp_q_ = 0;
						double e0qq_ = 0;

						//calculate Cpq\Cp_q_
						for (int temp = 0; temp < 3; temp ++) {
							double sum1;
							double sum2;
							if (LeftOrRight) {
								sum1 = static_cast<double>(LeftLab.at<Vec3b>(i, j)[temp] - LeftLab.at<Vec3b>(x, y + d)[temp]);
								sum2 = static_cast<double>(RightLab.at<Vec3b>(i, j - d)[temp] - RightLab.at<Vec3b>(x, y)[temp]);
							}
							else {
								sum1 = static_cast<double>(RightLab.at<Vec3b>(i, j)[temp] - RightLab.at<Vec3b>(x, y + d)[temp]);
								sum2 = static_cast<double>(LeftLab.at<Vec3b>(i, j - d)[temp] - LeftLab.at<Vec3b>(x, y)[temp]);
							}
							Cpq += pow(sum1, 2);
							Cp_q_ += pow(sum2, 2);
						}
						Cpq = sqrt(Cpq);
						Cp_q_ = sqrt(Cp_q_);
						
						//calculate Gpq\Gp_q_
						Gpq = sqrt(pow((x - i), 2) + pow((y + d - j), 2));
						Gp_q_ = sqrt(pow((x - i), 2) + pow((y - j + d), 2));

						//gain value of Wpq\Wp_q_
						Wpq = exp( - (Cpq / rc + Gpq / rp));
						Wp_q_ = exp( - (Cp_q_ / rc + Gp_q_ / rp));

						//calculate e0pq\e0p_q_
						for (int temp = 0; temp < 3; temp ++) {
							if (LeftOrRight)
								e0qq_ += fabs(static_cast<double>(Left.at<Vec3b>(x, y + d)[temp] - Right.at<Vec3b>(x, y)[temp]));
							else
								e0qq_ += fabs(static_cast<double>(Right.at<Vec3b>(x, y + d)[temp] - Left.at<Vec3b>(x, y)[temp]));
						}

						tempZ += Wpq * Wp_q_ * e0qq_;
						tempM += Wpq * Wp_q_;
					}
				}

				double result = tempZ / tempM;
				if (!LeftOrRight)d = -d;
				v.push_back(make_pair(result, d));
			}
			double min = DBL_MAX;
			int index = 0;
			for (int k = 0; k < v.size(); k ++) {
				if (v[k].first - min < 0) {
					min = v[k].first;
					index = v[k].second;
				}
			}
			output.ptr<uchar>(i)[j] = static_cast<uchar>(index * 3);
		}
	}
	return output;
}