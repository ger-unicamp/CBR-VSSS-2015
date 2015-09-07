/**
 * Main
 **/

// Bibliotecas C++
#include <iostream>
#include <string>
#include <vector>

// Bibliotecas C
#include <stdio.h>
#include <stdlib.h>

// Bibliotecas Externas
// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
// Arduino Serial
#include "arduino-serial-lib.c"

// Funções
#include "constantes.cpp"
#include "utils.hpp"
#include "inits.hpp"
#include "campo.hpp"

// Classes

// Namespaces
using namespace cv;
using namespace std;

void createTrackbarsGlobal();

//const int alpha_slider_max = 100;
//int alpha_slider;
//double alpha;
//double beta;

void on_trackbar(int, void*)
{
//	alpha = (double) alpha_slider/alpha_slider_max ;
//	beta = ( 1.0 - alpha );
//
//	addWeighted( imgOriginal, alpha, imgThresholded, beta, 0.0, imgThresholded);
//
//	imshow( "Control", imgThresholded );
}

int main( int argc, char* argv[] )
{
	cout << "Init video: Init" << endl;
	VideoCapture cap(0); //capture the video from webcam

	if ( !cap.isOpened() ) {  // if not success, exit program
		cout << "Cannot open the web cam" << endl;
		return -1;
	}
	cout << "Init video: End" << endl;

    char serialPort[] = "/dev/tty.usbmodem1421";

	// Variáveis para quando for o jogo
	int disableTrackbars = TRUE;

	// Variáveis relativas ao campo;
	int statusCampo = CAMPO_NAO_ENCONTRADO;
	int achouCampo;
	Point superiorEsq, inferiorEsq, superiorDir, inferiorDir;

	int contrast = -200;

	// HSV Components
	int iLowH = 3;
	int iHighH = 180;

	int iLowS = 2;
	int iHighS = 245;

	int iLowV = 5;
	int iHighV = 240;

	int kernelSize = 2;
	int kernelMorphological = 1;

	// Canny
	int epsilon = 5;
	int threshold = 120;
	int thresholdProp = 1;
	int apertureSize = 2;
	int L2gradient = TRUE;

	int areaPlayerL = 350;
	int areaPlayerH = 1300;

	int vPlayerL = 3;
	int vPlayerH = 9;

	int vPlayerIdL = 1;
	int vPlayerIdH = 2;

	initParameters();

  	if (!disableTrackbars) {
  		createTrackbarsGlobal();

  		// Trackbars com variáveis locais

		createTrackbar("LowH", "Control", &iLowH, 180, on_trackbar); //Hue (0 - 180)
		createTrackbar("HighH", "Control", &iHighH, 180, on_trackbar);

		createTrackbar("LowS", "Control", &iLowS, 255, on_trackbar); //Saturation (0 - 255)
		createTrackbar("HighS", "Control", &iHighS, 255, on_trackbar);

		createTrackbar("LowV", "Control", &iLowV, 255, on_trackbar);//Value (0 - 255)
		createTrackbar("HighV", "Control", &iHighV, 255, on_trackbar);

  		// TODO: Tornar as variáveis de funções de tratamento de imagem em variáveis globais, para fazer esquema de salvar em arquivo.

		// createTrackbar("contrast", "Control", &contrast, 200);
		// createTrackbar("ksize", "Control", &kernelSize, 20);
		// createTrackbar("kmorp", "Control", &kernelMorphological, 20);
		// createTrackbar("epsilon", "Control", &epsilon, 20);
		// createTrackbar("threshold", "Control", &threshold, 200);
		// createTrackbar("thresholdProp", "Control", &thresholdProp, 1);
		// createTrackbar("apertureSize", "Control", &apertureSize, 5);
		// createTrackbar("L2gradient", "Control", &L2gradient, 1);
	}

	while (true)
	{
		int campoEsquerdo = -1;
		int campoDireito = -1;
		int swapCampo;
		vector<vector<Point> > contours;
		vector<vector<Point> > contoursOut;
		vector<Vec4i> hierarchy;
		// RNG rng(12345);

		Mat imgOriginal, // Imagem Original - Nunca é alterada!
			imgAnalise, // Imagem utilizada para ver os resultados de identificação na imagem original.
			imgTransformada, // Imagem transformada do campo, após este ser localizado
			imgYCrCb,
			imgXYZ,
			imgHLS,
			imgLab,
			imgLuv,
			imgHSV, // Imagem com as componentes HSV
			imgThresholded, // Imagem a qual são aplicados filtros para encontrar os polígonos na imagem
			imgContrastRGB,
			imgContrastHSV,
			imgPlayersRGB = Mat::zeros(imgThresholded.size(), CV_8UC3), // Imagem (RGB) apenas dos jogadores
			mask_image = Mat::zeros(imgThresholded.size(), CV_8UC3), // Imagem de máscara para filtrar os jogadores
			imgPlayersHSV = Mat::zeros(imgThresholded.size(), CV_8UC3); // Imagem (HSV) apenas dos jogadores

		vector<Mat> channelsYCrCb(3),
					channelsXYZ(3),
					channelsHLS(3),
					channelsLab(3),
					channelsLuv(3),
					channelsHSV(3);

		achouCampo = FALSE;

		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) { //if not success, break loop
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		// imgAnalise = imgOriginal;

/**
	Mat::convertTo
	Converts an array to another data type with optional scaling.

	C++: void Mat::convertTo(OutputArray m, int rtype, double alpha=1, double beta=0 ) const
	Parameters:	
	m – output matrix; if it does not have a proper size or type before the operation, it is reallocated.
	rtype – desired output matrix type or, rather, the depth since the number of channels are the same as the input has; if rtype is negative, the output matrix will have the same type as the input.
	alpha – optional scale factor.
	beta – optional delta added to the scaled values.
**/

		// imgOriginal.convertTo(imgContrastRGB, -1, 1, contrast);
		// cvtColor(imgContrastRGB, imgContrastHSV, COLOR_BGR2HSV);

		/** Tipos de Luzes
		 *	Luz acromática - seu único atributo é a intensidade (ou a quantidade);
		 *	Luz cromática  - engloba o espectro de energia eletromagnética visível.
		 */

		/** YCrCb
		 * 	Modelo de representação da cor dedicado ao vídeo analógico.
		 *	O parâmero Y representa a luminância (ou seja a informação a preto e branco),
		 *	enquanto U e V permitem representar a corminância, ou seja, a informação sobre a cor.
		 */
		cvtColor(imgOriginal, imgYCrCb, COLOR_BGR2YCrCb);

		/** XYZ
		 *	O Y representa o valor de luminância (brilho), Z como próximo ao estímulo azul e X uma
		 * 	mistura (combinação linear) de estímulos escolhida para ser não negativo.
		 */
		cvtColor(imgOriginal, imgXYZ, COLOR_BGR2XYZ);

		/** HLS
		 *	Hue (matiz) - Define o componente de cor;
		 *	Lightness (brilho)
		 *	Saturation (saturação) - define o quão "pura" é a cor, ou se ela está misturada com
		 *		outras cores (complementar), tornando-se mais pálida.
		 */
		cvtColor(imgOriginal, imgHLS, COLOR_BGR2HLS);

		/** Lab e Luv
		 *	L = 0, produz preto; L = 100, branco difuso;
		 *	a < 0 indica cor próxima ao verde e a > 0 cor próxima ao magenta;
		 *	b < 0 indica cor próxima ao azul e b > 0 cor próxima ao amarelo.
		 */
		cvtColor(imgOriginal, imgLab, COLOR_BGR2Lab);
		cvtColor(imgOriginal, imgLuv, COLOR_BGR2Luv);

		/** HSV
		 *	Hue (matiz) - Define o componente de cor;
		 *	Saturation (saturação) - define o quão "pura" é a cor, ou se ela está misturada com
		 *		outras cores (complementar), tornando-se mais pálida.
		 *	Value (valor/brilho) - define a quantidade de luz na mistura, quanto mais luz mais
		 *		clara a cor (na ausência de valor, a imagem é toda preta)
		 */
		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		resize(imgOriginal, imgOriginal, Size(0,0), 0.3, 0.3, INTER_LINEAR);
		resize(imgYCrCb, imgYCrCb, Size(0,0), 0.3, 0.3, INTER_LINEAR);
		resize(imgXYZ, imgXYZ, Size(0,0), 0.3, 0.3, INTER_LINEAR);
		resize(imgHLS, imgHLS, Size(0,0), 0.3, 0.3, INTER_LINEAR);
		resize(imgLab, imgLab, Size(0,0), 0.3, 0.3, INTER_LINEAR);
		resize(imgLuv, imgLuv, Size(0,0), 0.3, 0.3, INTER_LINEAR);
		resize(imgHSV, imgHSV, Size(0,0), 0.3, 0.3, INTER_LINEAR);

		split(imgYCrCb, channelsYCrCb);
		split(imgXYZ, channelsXYZ);
		split(imgHLS, channelsHLS);
		split(imgLab, channelsLab);
		split(imgLuv, channelsLuv);
		split(imgHSV, channelsHSV);

/*********
		vector<Mat> channels; 
       	Mat imgHistEqualized;

       	cvtColor(imgOriginal, imgHistEqualized, COLOR_BGR2YCrCb); //change the color image from BGR to YCrCb format
       	split(imgHistEqualized, channels); //split the image into channels
       	equalizeHist(channels[0], channels[0]); //equalize histogram on the 1st channel (Y)
   		merge(channels, imgHistEqualized); //merge 3 channels including the modified 1st channel into one image
      	cvtColor(imgHistEqualized, imgHistEqualized, COLOR_YCrCb2BGR); //change the color image from YCrCb to BGR format (to display image properly)

       	// namedWindow("Histogram Equalized", CV_WINDOW_AUTOSIZE);
       	// imshow("Histogram Equalized", imgHistEqualized);

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

		//morphological opening (removes small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(2*kernelMorphological+1, 2*kernelMorphological+1)) );
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(2*kernelMorphological+1, 2*kernelMorphological+1)) );

		//morphological closing (removes small holes from the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(2*kernelMorphological+1, 2*kernelMorphological+1)) );
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(2*kernelMorphological+1, 2*kernelMorphological+1)) );

		//Calculate the moments of the thresholded image
//		Moments oMoments = moments(imgThresholded);
//
//		double dM01 = oMoments.m01;
//		double dM10 = oMoments.m10;
//		double dArea = oMoments.m00;

		// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero
//		if (dArea > 10000)
//		{
//			//calculate the position of the ball
//			int posX = dM10 / dArea;
//			int posY = dM01 / dArea;
//
//			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
//			{
//				//Draw a red line from the previous point to the current point
//				line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0,0,255), 2);
//			}
//
//			iLastX = posX;
//			iLastY = posY;
//		}

		blur(imgOriginal, imgOriginal, Size(2*kernelSize+1, 2*kernelSize+1));
		blur(imgThresholded, imgThresholded, Size(2*kernelSize+1, 2*kernelSize+1));

		/// Detect edges using canny
		Canny(imgThresholded, imgThresholded, threshold, threshold * (2 + thresholdProp), 2*apertureSize+1, L2gradient);

		/// Find contours
		findContours(imgThresholded, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

		contoursOut.resize(contours.size());
	    for( size_t k = 0; k < contours.size(); k++ ) {
	        approxPolyDP(Mat(contours[k]), contoursOut[k], epsilon, true);
	        Scalar color = COR_VERMELHO;

        	int area = fabs(contourArea(contours[k], 0));
	
    		Point pMedio = calculaPontoMedio(contoursOut[k]);

			if (statusCampo == CAMPO_NAO_ENCONTRADO ||
				(statusCampo == CAMPO_ENCONTRADO &&
				 (pMedio.x >= superiorEsq.x - MARGEM_DE_ERRO_CAMPO && pMedio.x >= inferiorEsq.x - MARGEM_DE_ERRO_CAMPO && 
				  pMedio.x <= superiorDir.x + MARGEM_DE_ERRO_CAMPO && pMedio.x <= inferiorDir.x + MARGEM_DE_ERRO_CAMPO &&
				  pMedio.y >= superiorEsq.y - MARGEM_DE_ERRO_CAMPO && pMedio.y >= superiorDir.y - MARGEM_DE_ERRO_CAMPO && 
			   	  pMedio.y <= inferiorEsq.y + MARGEM_DE_ERRO_CAMPO && pMedio.y <= inferiorDir.y + MARGEM_DE_ERRO_CAMPO))) {

				int r = imgContrastRGB.at<Vec3b>(pMedio.x, pMedio.y)[0];
				int g = imgContrastRGB.at<Vec3b>(pMedio.x, pMedio.y)[1];
				int b = imgContrastRGB.at<Vec3b>(pMedio.x, pMedio.y)[2];

				int h = imgHSV.at<Vec3b>(pMedio.x, pMedio.y)[0];
				int s = imgHSV.at<Vec3b>(pMedio.x, pMedio.y)[1];
				int v = imgHSV.at<Vec3b>(pMedio.x, pMedio.y)[2];

		      	if ((area >= areaBallL && area <= areaBallH) && 
		      		(h >= hBallL && h <= hBallH && s >= sBallL && s <= sBallH)) { // BOLA
	        		color = COR_LARANJA;
	        	} else if (statusCampo == CAMPO_NAO_ENCONTRADO && area >= areaAreaL && area <= areaAreaH) { // ÁREA DO CAMPO				
					if (contoursOut[k].size() == 4){
						if (campoEsquerdo == -1 && contoursOut[k][0].x < 640) {
							campoEsquerdo = k;
							achouCampo++;
						} else if (campoDireito == -1 && contoursOut[k][0].x >= 640) {
							campoDireito = k;
							achouCampo++;
						} else {
							color = COR_VERDE_ESCURO;
						}
					} 
				} else if (area >= areaPlayerL && area <= areaPlayerH) { // JOGADOR
	        	// } else if (contoursOut[k].size() >= vPlayerL && contoursOut[k].size() <= vPlayerH) { // JOGADOR
		        // 		   (contoursOut[k].size() >= vPlayerIdL && contoursOut[k].size() <= vPlayerIdH)) {

					// if (h >= hBlueL && h <= hBlueH && s >= sBlueL && s <= sBlueH && v >= vBlueL && v <= vBlueH) {
					if (r >= rBlueL && r <= rBlueH && g >= gBlueL && g <= gBlueH && b >= bBlueL && b <= bBlueH) {
						color = COR_AZUL;
					} else if (h >= hYellowL && h <= hYellowH && s >= sYellowL && s <= sYellowH && v >= vYellowL && v <= vYellowH) {
						color = COR_AMARELO;
					} else if (h >= hGreenL && h <= hGreenH && s >= sGreenL && s <= sGreenH && v >= vGreenL && v <= vGreenH) {
						color = COR_VERMELHO;
					} else {
						color = COR_CINZA;
					}

					drawContours(mask_image, contours, k, color, FILLED);
					imgOriginal.copyTo(imgPlayersRGB, mask_image);

				 	// Convert BGR to HSV
				    cvtColor(imgPlayersRGB, imgPlayersHSV, COLOR_BGR2HSV);

			    	drawContours(imgOriginal, contours, k, color, FILLED);

				    // Threshold the HSV image to get only blue colors
				    // inRange(imgPlayersHSV, lower_blue, upper_blue, mask_image);
				    // inRange(imgPlayersHSV, lower_yellow, upper_yellow, mask_image);
				    // inRange(imgPlayersHSV, lower_color, upper_color, mask_image);

				    // Bitwise-AND mask and original image
				    // res = cv2.bitwise_and(frame,frame, mask= mask)
				    // imgOriginal.copyTo(imgPlayersRGB, mask_image);

					// drawContours(mask_image, contours, k, Scalar(255), FILLED);
					// imgOriginal.copyTo(imgPlayersRGB, mask_image);
					
					// int b = imgPlayersRGB.at<Vec3b>(i,j)[0];
					// int g = imgPlayersRGB.at<Vec3b>(i,j)[1];
					// int r = imgPlayersRGB.at<Vec3b>(i,j)[2];

	        		// Cores dos times
	        		// Azul   : HSV(161,6,100); RGB(239,255,250); COLOR PRINT BLUE(255, 0, 0)
	        		// Amarelo: HSV(120,1,100); RGB(251,254,251); COLOR PRINT AMARELA(0, 255, 255)

	    //  			int threshold_h = 3;
	    //  			int threshold_s = 5;
	    //  			color = Scalar(0, 0, 255); // Vermelho
					// if (((pMedio.x < 427 - 1 || pMedio.x > 427 + 1) && (pMedio.y < 425 - 1 || pMedio.y > 425 + 1))) {
					// 	cout << "Ponto medio[" << pMedio.x << "," << pMedio.y << "] - HSV(" << h << "," << s << "," << v << ")" << endl;
					// }
	     
					// if ((h >= 161 - threshold_h && h <= 161 + threshold_h) && (s >= 6 - threshold_s && s <= 6 + threshold_s)) {
					// 	color = Scalar(255, 0, 0); // AZUL
					// } else if ((h >= 120 - threshold_h && h <= 120 + threshold_h) && (s >= 1 - threshold_s && s <= 1 + threshold_s)) {
					// 	color = Scalar(0, 255, 255); // AMARELO
					// } else 
					// if ((h >= 87 - threshold_h && h <= 87 + threshold_h) && (s >= 47 - threshold_s && s <= 47 + threshold_s)) {
					// 	color = Scalar(0,255,0); // VERDE
					// }

	        		// Cor individual
	        		// Verde  : HSV(87,47,255); RGB(0,255,0);
		        } else {
		        	color = COR_CINZA;
		        }
		        drawContours( imgOriginal, contours, k, color, 2, 8, hierarchy, 0, Point() );
		    }
		}

		if (achouCampo == 2 && statusCampo == CAMPO_NAO_ENCONTRADO) {
			if (contoursOut[campoDireito][0].x < contoursOut[campoEsquerdo][0].x) {
				swapCampo = campoEsquerdo;
				campoEsquerdo = campoDireito;
				campoDireito = swapCampo;
			}

			Point aEsq, bEsq, cEsq, dEsq, aDir, bDir, cDir, dDir;

			defineABCD(contoursOut[campoEsquerdo], &aEsq, &bEsq, &cEsq, &dEsq); // Seta os pontos A, B, C e D do campo esquerdo.
			defineABCD(contoursOut[campoDireito], &aDir, &bDir, &cDir, &dDir);  // Seta os pontos A, B, C e D do campo direito.	
			defineCantos(aEsq, cEsq, bDir, dDir, &superiorEsq, &inferiorEsq, &superiorDir, &inferiorDir);
			imgTransformada = transformaCampo(superiorEsq, inferiorEsq, superiorDir, inferiorDir, imgOriginal);
			statusCampo = CAMPO_ENCONTRADO;
		}

		if (statusCampo == CAMPO_ENCONTRADO) {
			vector<Point> pts;
			pts.push_back(superiorEsq);
			pts.push_back(inferiorEsq);
			pts.push_back(inferiorDir);
			pts.push_back(superiorDir);
			polylines(imgOriginal, pts, 1, COR_VERDE, 2, 8, 0);

		    // namedWindow("imgTransformada", WINDOW_AUTOSIZE);
		    // imshow("imgTransformada", imgTransformada); //show the thresholded image
		}
		// copy only non-zero pixels from your image to original image
		// for (int i = 0; i < imgPlayersRGB.rows; i++) {
		// 	for (int j = 0; j < imgPlayersRGB.cols; j++) {
		// 		int B = imgPlayersRGB.at<Vec3b>(i,j)[0];
		// 		int G = imgPlayersRGB.at<Vec3b>(i,j)[1];
		// 		int R = imgPlayersRGB.at<Vec3b>(i,j)[2];
		// 		//if (B != 0 || G != 0 || R != 0) {
		// 			//cout << "[" << i << "," << j << "] = RGB(" << R << "," << G << "," << B << ");" << endl;
		// 			cout << R << " " <<  G << " " << B << " ";
		// 		//}
		// 	}
		// 	cout << endl;
		// }
		// cout << "END" << endl;
**********/

	    namedWindow("imgOriginal", WINDOW_AUTOSIZE);
	    imshow("imgOriginal", imgOriginal); //show the thresholded image
/*
	    // YCrCb
	    namedWindow("imgYCrCb", WINDOW_AUTOSIZE);
	    imshow("imgYCrCb", imgYCrCb);

	    namedWindow("channelsYCrCb[0]", WINDOW_AUTOSIZE);
	    imshow("channelsYCrCb[0]", channelsYCrCb[0]);

	    namedWindow("channelsYCrCb[1]", WINDOW_AUTOSIZE);
	    imshow("channelsYCrCb[1]", channelsYCrCb[1]);

	    namedWindow("channelsYCrCb[2]", WINDOW_AUTOSIZE);
	    imshow("channelsYCrCb[2]", channelsYCrCb[2]);

	    // XYZ
	    namedWindow("imgXYZ", WINDOW_AUTOSIZE);
	    imshow("imgXYZ", imgXYZ);

	    namedWindow("channelsXYZ[0]", WINDOW_AUTOSIZE);
	    imshow("channelsXYZ[0]", channelsXYZ[0]);

	    namedWindow("channelsXYZ[1]", WINDOW_AUTOSIZE);
	    imshow("channelsXYZ[1]", channelsXYZ[1]);

	    namedWindow("channelsXYZ[2]", WINDOW_AUTOSIZE);
	    imshow("channelsXYZ[2]", channelsXYZ[2]);

	    // HLS
	    namedWindow("imgHLS", WINDOW_AUTOSIZE);
	    imshow("imgHLS", imgHLS);

	    namedWindow("channelsHLS[0]", WINDOW_AUTOSIZE);
	    imshow("channelsHLS[0]", channelsHLS[0]);

	    namedWindow("channelsHLS[1]", WINDOW_AUTOSIZE);
	    imshow("channelsHLS[1]", channelsHLS[1]);

	    namedWindow("channelsHLS[2]", WINDOW_AUTOSIZE);
	    imshow("channelsHLS[2]", channelsHLS[2]);

	    // Lab
	    namedWindow("imgLab", WINDOW_AUTOSIZE);
	    imshow("imgLab", imgLab);

	    namedWindow("channelsLab[0]", WINDOW_AUTOSIZE);
	    imshow("channelsLab[0]", channelsLab[0]);

	    namedWindow("channelsLab[1]", WINDOW_AUTOSIZE);
	    imshow("channelsLab[1]", channelsLab[1]);

	    namedWindow("channelsLab[2]", WINDOW_AUTOSIZE);
	    imshow("channelsLab[2]", channelsLab[2]);

	    // Luv
	    namedWindow("imgLuv", WINDOW_AUTOSIZE);
	    imshow("imgLuv", imgLuv);

	    namedWindow("channelsLuv[0]", WINDOW_AUTOSIZE);
	    imshow("channelsLuv[0]", channelsLuv[0]);

	    namedWindow("channelsLuv[1]", WINDOW_AUTOSIZE);
	    imshow("channelsLuv[1]", channelsLuv[1]);

	    namedWindow("channelsLuv[2]", WINDOW_AUTOSIZE);
	    imshow("channelsLuv[2]", channelsLuv[2]);

	    // HSV
	    namedWindow("imgHSV", WINDOW_AUTOSIZE);
	    imshow("imgHSV", imgHSV);

	    namedWindow("channelsHSV[0]", WINDOW_AUTOSIZE);
	    imshow("channelsHSV[0]", channelsHSV[0]);

	    namedWindow("channelsHSV[1]", WINDOW_AUTOSIZE);
	    imshow("channelsHSV[1]", channelsHSV[1]);

	    namedWindow("channelsHSV[2]", WINDOW_AUTOSIZE);
	    imshow("channelsHSV[2]", channelsHSV[2]);
*/

		// namedWindow("imgHistEqualized", WINDOW_NORMAL);
		// imshow( "imgHistEqualized", imgHistEqualized);

		// namedWindow("imgThresholded", WINDOW_NORMAL);
		// imshow( "imgThresholded", imgThresholded);

		// namedWindow("imgPlayersRGB", WINDOW_NORMAL);
		// imshow( "imgPlayersRGB", imgOriginal);

		// namedWindow("imgPlayersHSV", WINDOW_NORMAL);
		// imshow( "imgPlayersHSV", imgPlayersHSV);

		// imgPlayersRGB.release();
		// imgPlayersHSV.release();
		// mask_image.release();

		if (saveBlueParametersOfTrackbar) {
			ofstream blueParametersNewFile;
			blueParametersNewFile.open("parameters/blue-color-parameters.txt");
			blueParametersNewFile << "hL=" << hBlueL << endl;
			blueParametersNewFile << "hH=" << hBlueH << endl;
			blueParametersNewFile << "sL=" << sBlueL << endl;
			blueParametersNewFile << "sH=" << sBlueH << endl;
			blueParametersNewFile << "vL=" << sBlueL << endl;
			blueParametersNewFile << "vH=" << sBlueH << endl;
			blueParametersNewFile << "rL=" << rBlueL << endl;
			blueParametersNewFile << "rH=" << rBlueH << endl;
			blueParametersNewFile << "gL=" << gBlueL << endl;
			blueParametersNewFile << "gH=" << gBlueH << endl;
			blueParametersNewFile << "bL=" << bBlueL << endl;
			blueParametersNewFile << "bH=" << bBlueH << endl;
			blueParametersNewFile.close();
			saveBlueParametersOfTrackbar = FALSE;
		}

		if (saveYellowParametersOfTrackbar) {
			ofstream yellowParametersNewFile;
			yellowParametersNewFile.open("parameters/yellow-color-parameters.txt");
			yellowParametersNewFile << "hL=" << hYellowL << endl;
			yellowParametersNewFile << "hH=" << hYellowH << endl;
			yellowParametersNewFile << "sL=" << sYellowL << endl;
			yellowParametersNewFile << "sH=" << sYellowH << endl;
			yellowParametersNewFile << "vL=" << sYellowL << endl;
			yellowParametersNewFile << "vH=" << sYellowH << endl;
			yellowParametersNewFile << "rL=" << rYellowL << endl;
			yellowParametersNewFile << "rH=" << rYellowH << endl;
			yellowParametersNewFile << "gL=" << gYellowL << endl;
			yellowParametersNewFile << "gH=" << gYellowH << endl;
			yellowParametersNewFile << "bL=" << bYellowL << endl;
			yellowParametersNewFile << "bH=" << bYellowH << endl;
			yellowParametersNewFile.close();
			saveYellowParametersOfTrackbar = FALSE;
		}

		if (saveBallParametersOfTrackbar) {
			ofstream ballParametersNewFile;
			ballParametersNewFile.open("parameters/ball-parameters.txt");
			ballParametersNewFile << "areaL=" << areaBallL << endl;
			ballParametersNewFile << "areaH=" << areaBallH << endl;
			ballParametersNewFile.close();
			saveBallParametersOfTrackbar = FALSE;
		}

		if (saveAreaParametersOfTrackbar) {
			ofstream areaParametersNewFile;
			areaParametersNewFile.open("parameters/area-parameters.txt");
			areaParametersNewFile << "areaL=" << areaAreaL << endl;
			areaParametersNewFile << "areaH=" << areaAreaH << endl;
			areaParametersNewFile.close();
			saveAreaParametersOfTrackbar = FALSE;
		}

		if (waitKey(30) == 27) { //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	return 0;
}

/**
 *     Função: createTrackbarsGlobal
 * Parâmetros: -
 *    Retorno: -
 *  Descrição: Cria trackbars de variáveis globais.
 *    Exemplo: -
 **/
void createTrackbarsGlobal() {
	// TODO: Descobrir se é possível ordenar os itens de cada trackbar, pois ele não coloca na ordem que é colocado abaixo, e mistrura tudo.

	// BOLA
	// namedWindow("Control Ball", WINDOW_NORMAL);
	// createTrackbar("saveBallParametersOfTrackbar", "Control Ball", &saveBallParametersOfTrackbar, 1);
	// createTrackbar("areaBallL", "Control Ball", &areaBallL, 1000);
	// createTrackbar("areaBallH", "Control Ball", &areaBallH, 1000);
	// createTrackbar("hBallL", "Control Ball", &hBallL, 180);
	// createTrackbar("hBallH", "Control Ball", &hBallH, 180);
	// createTrackbar("sBallL", "Control Ball", &sBallL, 255);
	// createTrackbar("sBallH", "Control Ball", &sBallH, 255);

	// ÁREA DO CAMPO
	// namedWindow("Control Area", WINDOW_NORMAL);
	// createTrackbar("saveAreaParametersOfTrackbar", "Control Area", &saveAreaParametersOfTrackbar, 1);
	// createTrackbar("areaAreaL", "Control Area", &areaAreaL, 40000);
	// createTrackbar("areaAreaH", "Control Area", &areaAreaH, 40000);

	// JOGADORES
	// namedWindow("Control Player", WINDOW_NORMAL);
	// createTrackbar("vPlayerL", "Control Player", &vPlayerL, 40);
	// createTrackbar("vPlayerH", "Control Player", &vPlayerH, 40);
	// createTrackbar("areaPlayerL", "Control Player", &areaPlayerL, 1500);
	// createTrackbar("areaPlayerH", "Control Player", &areaPlayerH, 1500);

	// JOGADORES AZUL (COR TIME)
	namedWindow("Control Blue", WINDOW_NORMAL);
	createTrackbar("saveBlueParametersOfTrackbar", "Control Blue", &saveBlueParametersOfTrackbar, 1);
	// createTrackbar("hBlueL", "Control Blue", &hBlueL, 180);
	// createTrackbar("hBlueH", "Control Blue", &hBlueH, 180);
	// createTrackbar("sBlueL", "Control Blue", &sBlueL, 255);
	// createTrackbar("sBlueH", "Control Blue", &sBlueH, 255);
	// createTrackbar("vBlueL", "Control Blue", &vBlueL, 255);
	// createTrackbar("vBlueH", "Control Blue", &vBlueH, 255);
	createTrackbar("rBlueL", "Control Blue", &rBlueL, 255);
	createTrackbar("rBlueH", "Control Blue", &rBlueH, 255);
	createTrackbar("gBlueL", "Control Blue", &gBlueL, 255);
	createTrackbar("gBlueH", "Control Blue", &gBlueH, 255);
	createTrackbar("bBlueL", "Control Blue", &bBlueL, 255);
	createTrackbar("bBlueH", "Control Blue", &bBlueH, 255);

	// JOGADORES AMARELO (COR TIME)
	namedWindow("Control Yellow", WINDOW_NORMAL); //create a window called "Control"
	createTrackbar("saveYellowParametersOfTrackbar", "Control Yellow", &saveYellowParametersOfTrackbar, 1);
	// createTrackbar("hYellowL", "Control Yellow", &hYellowL, 180);
	// createTrackbar("hYellowH", "Control Yellow", &hYellowH, 180);
	// createTrackbar("sYellowL", "Control Yellow", &sYellowL, 255);
	// createTrackbar("sYellowH", "Control Yellow", &sYellowH, 255);
	createTrackbar("rYellowL", "Control Yellow", &rYellowL, 255);
	createTrackbar("rYellowH", "Control Yellow", &rYellowH, 255);
	createTrackbar("gYellowL", "Control Yellow", &gYellowL, 255);
	createTrackbar("gYellowH", "Control Yellow", &gYellowH, 255);
	createTrackbar("bYellowL", "Control Yellow", &bYellowL, 255);
	createTrackbar("bYellowH", "Control Yellow", &bYellowH, 255);

	// JOGADORES VERDE (COR IDENTIFICAÇÃO JOGADOR)
	// TODO: Fazer save dos parâmetros dessa cor.
	// namedWindow("Control Green", WINDOW_NORMAL);
	// createTrackbar("vPlayerIdL", "Control Green", &vPlayerIdL, 20);
	// createTrackbar("vPlayerIdH", "Control Green", &vPlayerIdH, 20);
	// createTrackbar("areaGreenL", "Control Green", &areaGreenL, 200);
	// createTrackbar("areaGreenH", "Control Green", &areaGreenH, 200);
	// createTrackbar("hGreenL", "Control Green", &hGreenL, 180);
	// createTrackbar("hGreenH", "Control Green", &hGreenH, 180);
	// createTrackbar("sGreenL", "Control Green", &sGreenL, 255);
	// createTrackbar("sGreenH", "Control Green", &sGreenH, 255);
}

void sendCommands(char serialPort[], int robot, int directionRight, int speedRight, int directionLeft, int speedLeft) {
    char data[] = {'S', robot, '#', directionRight, '#', speedRight, '#', directionLeft, '#', speedLeft, 'E'};
    FILE *file;
    file = fopen(serialPort, "w"); // Opening usb device file. 
    int i = 0;
    for (i = 0 ; i < 13 ; i++) {
        fprintf(file, "%c", data[i]); // Writing to the file.
        //printf("%c\n", (char) data[i]);
    }
    fclose(file);
}
