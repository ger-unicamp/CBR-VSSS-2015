/**
 * Main
 **/

// Bibliotecas C++
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

// Bibliotecas C
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Bibliotecas Externas
// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

// Funções
#include "constantes.cpp"
#include "utils.hpp"
#include "inits.hpp"
#include "campo.hpp"
#include "estrategia.h"

// Classes

// Namespaces
using namespace cv;
using namespace std;

void createTrackbarsGlobal();
void saveTrackbarsParameters();
void defineJogadores(Point posJogadores[]);
void defineFrenteJogador(int jogador);
void sendCommands(int robot, int directionRight, int speedRight, int directionLeft, int speedLeft);
void jogaBonito();
void jogaLouco();

int main( int argc, char* argv[] )
{
	bool modoJogo = false;
	bool inverteCampo = false;

	// Variáveis para quando for o jogo
	int disableTrackbars = FALSE;

	cout << "Init video: Init" << endl;

	VideoCapture cap(0); //capture the video from webcam

	if ( !cap.isOpened() ) {  // if not success, exit program
		cout << "Cannot open the web cam" << endl;
		return -1;
	}
	cout << "Init video: End" << endl;

	// Variáveis relativas ao campo;
	int statusCampo = CAMPO_NAO_ENCONTRADO;
	int achouCampoOpcao1, achouCampoOpcao2;
	Point superiorEsq, inferiorEsq, superiorDir, inferiorDir;

	int contrast = -200;

	Point posJogadores[TAMANHO_VETOR_JOGADORES];
	int countJogadores = 0;

	// HSV Components
	int iLowH = 0;
	int iHighH = 180;

	int iLowS = 0;
	int iHighS = 255;

	int iLowV = 200;
	int iHighV = 255;

	int kernelSize = 2;
	// Para identificar o Campo com o método 1, tem que começar esse parâmetro com zero.
	// Com o método 2, tem que começar com 3. O método 1 tem um problema, que é quando tem algum
	// jogador sobre alguma linha das grandes áreas. O método 2 é melhor por ser menos sensível
	// a outros fatores (pelo menos vistos até agora).
	int kernelMorphological = 0;

	// Canny
	int epsilon = 5;
	int threshold = 120;
	int thresholdProp = 1;
	int apertureSize = 2;
	int L2gradient = TRUE;

	int countPrints = 2;

	int alpha = 100;
	int beta = 0;

	initParameters();

	// int aux = 0;

  	if (!disableTrackbars && !modoJogo) {
  		createTrackbarsGlobal();

  		// Trackbars com variáveis locais

  		namedWindow("Control", WINDOW_NORMAL);
		// createTrackbar("LowH", "Control", &iLowH, 180, on_trackbar); //Hue (0 - 180)
		// createTrackbar("HighH", "Control", &iHighH, 180, on_trackbar);

		// createTrackbar("LowS", "Control", &iLowS, 255, on_trackbar); //Saturation (0 - 255)
		// createTrackbar("HighS", "Control", &iHighS, 255, on_trackbar);

		// createTrackbar("LowV", "Control", &iLowV, 255, on_trackbar);//Value (0 - 255)
		// createTrackbar("HighV", "Control", &iHighV, 255, on_trackbar);

  		// TODO: Tornar as variáveis de funções de tratamento de imagem em variáveis globais, para fazer esquema de salvar em arquivo.

		// createTrackbar("contrast", "Control", &contrast, 200);
		// createTrackbar("ksize", "Control", &kernelSize, 20);
		createTrackbar("kmorp", "Control", &kernelMorphological, 20);
		// createTrackbar("epsilon", "Control", &epsilon, 20);
		// createTrackbar("threshold", "Control", &threshold, 200);
		// createTrackbar("thresholdProp", "Control", &thresholdProp, 1);
		// createTrackbar("apertureSize", "Control", &apertureSize, 5);
		// createTrackbar("L2gradient", "Control", &L2gradient, 1);


		createTrackbar("alpha", "Control", &alpha, 100);
		createTrackbar("beta", "Control", &beta, 100);
	}

	while (true)
	{
		// cout << aux++ << endl;
		int campoEsquerdo = -1;
		int campoDireito = -1;
		int limiteSuperior = -1;
		int limiteInferior = -1;
		int swapCampo;
		vector<vector<Point> > contours, contoursBall, contoursOut, contoursBallOut;
		vector<Vec4i> hierarchy;

		Mat imgOriginal, imgOriginalMenor, // Imagem Original - Nunca é alterada!
			imgAnalise, imgAnaliseMenor, // Imagem utilizada para ver os resultados de identificação na imagem original.
			// imgTransformada, // Imagem transformada do campo, após este ser localizado
			imgHSV, // Imagem com as componentes HSV
			imgHSVHist,
			imgHSVMod,
			// imgHLS, imgHLSMenor,
			imgThresholded, imgThresholdedMenor, // Imagem a qual são aplicados filtros para encontrar os polígonos na imagem
			// imgContrastRGB,
			// imgContrastHSV,
			imgBola, imgBolaMenor,
			imgTeste,
			// imgAzul, imgAzulMenor,
			// imgAmarelo, imgAmareloMenor,
			// imgRoxo, imgRoxoMenor,
			// imgPlayersRGB = Mat::zeros(imgThresholded.size(), CV_8UC3), // Imagem (RGB) apenas dos jogadores
			mask_image = Mat::zeros(imgThresholded.size(), CV_8UC3); // Imagem de máscara para filtrar os jogadores
			// imgPlayersHSV = Mat::zeros(imgThresholded.size(), CV_8UC3); // Imagem (HSV) apenas dos jogadores

		achouCampoOpcao1 = achouCampoOpcao2 = FALSE;

		
		cap.set(CAP_PROP_BRIGHTNESS, 0.2);
		cap.set(CAP_PROP_CONTRAST, 10);
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) { //if not success, break loop
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		// imgOriginal = imread("prints/tela07mod.png");

		countJogadores = 0;
		for (int i = 0; i < TAMANHO_VETOR_JOGADORES; i++) {
			posJogadores[i] = Point(0, 0);
		}

		// Point2f srcTri[3];
		// Point2f dstTri[3];
		// srcTri[0] = Point2f( 0,0 );
		// srcTri[1] = Point2f( imgOriginal.cols - 1, 0 );
		// srcTri[2] = Point2f( 0, imgOriginal.rows - 1 );

		// dstTri[0] = Point2f( imgOriginal.cols*0.0, imgOriginal.rows*0.33 );
		// dstTri[1] = Point2f( imgOriginal.cols*0.85, imgOriginal.rows*0.25 );
		// dstTri[2] = Point2f( imgOriginal.cols*0.15, imgOriginal.rows*0.7 );

		// Mat warp_mat( 2, 3, CV_32FC1 );
		// warpAffine(imgOriginal, imgOriginal, warp_mat, imgOriginal.size() );
		// warp_mat = getAffineTransform( srcTri, dstTri );
		if (inverteCampo) {
			flip(imgOriginal, imgOriginal, 1);
		}
		imgOriginal.copyTo(imgAnalise, mask_image);

		imgTeste = Mat::zeros( imgOriginal.size(), imgOriginal.type() );
		imgTeste = imgOriginal + Scalar(-75, -75, -75);
		// imgOriginal.convertTo(imgTeste, -1, alpha / 100, - beta);


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
		// imgAnalise.convertTo(imgContrastRGB, -1, 1, contrast);
		// cvtColor(imgContrastRGB, imgContrastHSV, COLOR_BGR2HSV);

		/*
		cvtColor(imgAnalise, imgGray, COLOR_BGR2GRAY);
		GaussianBlur(imgGray, imgGray, Size(9, 9), 2, 2);
		vector<Vec3f> circles;
    	HoughCircles(imgGray, circles, HOUGH_GRADIENT, 1, 20, 200, 1, 1, 100);
    	// cout << "circles.size(): " << circles.size() << endl;
    	for (size_t i = 0; i < circles.size(); i++) {
	         Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
	         int radius = cvRound(circles[i][2]);
	         // draw the circle center
	         circle(imgGray, center, 3, Scalar(0,255,0), -1, 8, 0);
	         // draw the circle outline
	         circle(imgGray, center, radius, Scalar(0,0,255), 3, 8, 0);
	    }
	    */

		cvtColor(imgAnalise, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		vector<Mat> channelsHSV;
		split(imgHSV, channelsHSV);
		// add(channelsHSV[0], Scalar(50), channelsHSV[0]);
		add(channelsHSV[1], Scalar(50), channelsHSV[1]);
		add(channelsHSV[2], Scalar(-50), channelsHSV[2]);
		merge(channelsHSV, imgHSVMod);
		// cvtColor(imgHSVMod, imgHSVMod, COLOR_HSV2BGR);

		/*** TESTE COM IMAGEM EQUALIZADA A PARTIR DE HISTOGRAMA ***/
		vector<Mat> channels;
       	Mat imgHistEqualized, imgHistEqualizedMenor;
       	cvtColor(imgAnalise, imgHistEqualized, COLOR_BGR2YCrCb); //change the color image from BGR to YCrCb format
       	split(imgHistEqualized, channels); //split the image into channels
       	equalizeHist(channels[0], channels[0]); //equalize histogram on the 1st channel (Y)
   		merge(channels, imgHistEqualized); //merge 3 channels including the modified 1st channel into one image
      	cvtColor(imgHistEqualized, imgHistEqualized, COLOR_YCrCb2BGR); //change the color image from YCrCb to BGR format (to display image properly)
      	cvtColor(imgHistEqualized, imgHSVHist, COLOR_BGR2HSV);
      	imgHSV = imgHSVMod;
/**
	inRange: Checks if array elements lie between the elements of two other arrays.

	C++: void inRange(InputArray src, InputArray lowerb, InputArray upperb, OutputArray dst)
	Parameters:	
	src – first input array.
	lowerb – inclusive lower boundary array or a scalar.
	upperb – inclusive upper boundary array or a scalar.
	dst – output array of the same size as src and CV_8U type.
**/

		// inRange(imgHSV, Scalar(15, 100, 100), Scalar(45, 255, 255), imgLaranja);
		inRange(imgHSV, Scalar(hBallL, sBallL, vBallL), Scalar(hBallH, sBallH, vBallH), imgBola);
		// inRange(imgHSV, Scalar(hBlueL, sBlueL, vBlueL), Scalar(hBlueH, sBlueH, vBlueH), imgAzul);
		// inRange(imgHSV, Scalar(hYellowL, sYellowL, vYellowL), Scalar(hYellowH, sYellowH, vYellowH), imgAmarelo);
		// inRange(imgHSV, Scalar(hPurpleL, sPurpleL, vPurpleL), Scalar(hPurpleH, sPurpleH, vPurpleH), imgRoxo);

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

		//morphological opening (removes small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(2 * kernelMorphological + 1, 2 * kernelMorphological + 1)) );
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(2 * kernelMorphological + 1, 2 * kernelMorphological + 1)) );

		//morphological closing (removes small holes from the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(2 * kernelMorphological + 1, 2 * kernelMorphological + 1)) );
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(2 * kernelMorphological + 1, 2 * kernelMorphological + 1)) );

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

		blur(imgAnalise, imgAnalise, Size(2*kernelSize+1, 2*kernelSize+1));
		blur(imgThresholded, imgThresholded, Size(2*kernelSize+1, 2*kernelSize+1));

		/// Detect edges using canny
		Canny(imgThresholded, imgThresholded, threshold, threshold * (2 + thresholdProp), 2 * apertureSize + 1, L2gradient);

		/// Find contours
		findContours(imgThresholded, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

		contoursOut.resize(contours.size());
	    for (size_t k = 0; k < contours.size(); k++ ) {
	        approxPolyDP(Mat(contours[k]), contoursOut[k], epsilon, true);
	        Scalar color = COR_CINZA;

        	int area = fabs(contourArea(contours[k], 0));
	
    		Point pMedio;
    		int diffX, diffY;
    		calculaPontoMedio(contoursOut[k], &pMedio, &diffX, &diffY);

			if (statusCampo == CAMPO_NAO_ENCONTRADO ||
				(statusCampo == CAMPO_ENCONTRADO &&
				 (pMedio.x >= superiorEsq.x - MARGEM_DE_ERRO_CAMPO && pMedio.x >= inferiorEsq.x - MARGEM_DE_ERRO_CAMPO && 
				  pMedio.x <= superiorDir.x + MARGEM_DE_ERRO_CAMPO && pMedio.x <= inferiorDir.x + MARGEM_DE_ERRO_CAMPO &&
				  pMedio.y >= superiorEsq.y - MARGEM_DE_ERRO_CAMPO && pMedio.y >= superiorDir.y - MARGEM_DE_ERRO_CAMPO && 
			   	  pMedio.y <= inferiorEsq.y + MARGEM_DE_ERRO_CAMPO && pMedio.y <= inferiorDir.y + MARGEM_DE_ERRO_CAMPO))) {
				// int r = imgContrastRGB.at<Vec3b>(pMedio.x, pMedio.y)[0];
				// int g = imgContrastRGB.at<Vec3b>(pMedio.x, pMedio.y)[1];
				// int b = imgContrastRGB.at<Vec3b>(pMedio.x, pMedio.y)[2];

				// int h = imgHSV.at<Vec3b>(pMedio.x, pMedio.y)[0];
				// int s = imgHSV.at<Vec3b>(pMedio.x, pMedio.y)[1];
				// int v = imgHSV.at<Vec3b>(pMedio.x, pMedio.y)[2];

		      	if (statusCampo == CAMPO_NAO_ENCONTRADO) { // ÁREA DO CAMPO
					if (area >= areaAreaL && area <= areaAreaH && contoursOut[k].size() == 4) { // Método 1: Determinando pela grande área.
						if (campoEsquerdo == -1 && pMedio.x < 640) {
							campoEsquerdo = k;
							achouCampoOpcao1++;
						} else if (campoDireito == -1 && pMedio.x >= 640) {
							campoDireito = k;
							achouCampoOpcao1++;
						} else {
							color = COR_VERDE_ESCURO;
						}
					} else if (diffX > 700) { // Método 2: Determinado pelos limites superior e inferior.
						if (limiteSuperior == -1 && pMedio.y < 360) {
							limiteSuperior = k;
							achouCampoOpcao2++;
						} else if (limiteInferior == -1 && pMedio.y >= 360) {
							limiteInferior = k;
							achouCampoOpcao2++;
						} else {
							color = COR_VERDE_ESCURO;
						}
					}
				} else if (diffX <= 300 && diffY <= 300 /*&& area >= 400 && area <= 700*/) { // JOGADOR
				// } else if (area >= areaJogadorL && area <= areaJogadorH) { // JOGADOR
					// drawContours(mask_image, contours, k, color, FILLED);
					// imgOriginal.copyTo(imgPlayersRGB, mask_image);

				 	// Convert BGR to HSV
				    // cvtColor(imgPlayersRGB, imgPlayersHSV, COLOR_BGR2HSV);

					if (area >= areaId1L && area <= areaId1H) {
						color = COR_AZUL;
						posJogador1[0] = pMedio;
					} else if (area > areaId2L && area <= areaId2H) {
						color = COR_AMARELO;
						posJogador2[0] = pMedio;
					} else if (area >= areaJogadorL && area <= areaJogadorH) {
						color = COR_VERMELHO;
						if (statusCampo == CAMPO_ENCONTRADO && countJogadores < TAMANHO_VETOR_JOGADORES) {
							bool altera = true;
							for (int i = 0; i < TAMANHO_VETOR_JOGADORES; i++) {
								if (posJogadores[i].x != 0 && posJogadores[i].y != 0) {
									int diffX = abs(pMedio.x - posJogadores[i].x);
									int diffY = abs(pMedio.y - posJogadores[i].y);
									if (diffX < 30 && diffY < 30)
										altera = false;
								}
							}
							if (altera) {
								posJogadores[countJogadores++] = pMedio;
							}
						}
					} else if (area >= areaId3L && area <= areaId3H) {
						color = COR_VERDE_ESCURO;
						posJogador3[0] = pMedio;
					} else {
						color = COR_CINZA;
					}

					if (!modoJogo)
			    		drawContours(imgAnalise, contours, k, color, FILLED);
			    	// cout << area << endl;

					// if (statusCampo == CAMPO_ENCONTRADO) {
					// 	int r = imgOriginal.at<Vec3b>(pMedio.x, pMedio.y)[0];
					// 	int g = imgOriginal.at<Vec3b>(pMedio.x, pMedio.y)[1];
					// 	int b = imgOriginal.at<Vec3b>(pMedio.x, pMedio.y)[2];
					// 	printf("(%d, %d) - RGB (%d, %d, %d)\n", pMedio.x, pMedio.y, r, g, b);

					// 	int h = imgHSV.at<Vec3b>(pMedio.x, pMedio.y)[0];
					// 	int s = imgHSV.at<Vec3b>(pMedio.x, pMedio.y)[1];
					// 	int v = imgHSV.at<Vec3b>(pMedio.x, pMedio.y)[2];
					// 	printf("(%d, %d) - HSV (%d, %d, %d)\n", pMedio.x, pMedio.y, h, s, v);

					// 	int y = imgHistEqualized.at<Vec3b>(pMedio.x, pMedio.y)[0];
					// 	int cr = imgHistEqualized.at<Vec3b>(pMedio.x, pMedio.y)[1];
					// 	int cb = imgHistEqualized.at<Vec3b>(pMedio.x, pMedio.y)[2];
					// 	printf("(%d, %d) - YCrCb (%d, %d, %d)\n", pMedio.x, pMedio.y, y, cr, cb);
					// }

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

		        if (!modoJogo)
		        	drawContours(imgAnalise, contours, k, color, 2, 8, hierarchy, 0, Point());
		    }
		}

		if ((achouCampoOpcao1 == 2 || achouCampoOpcao2 == 2) && statusCampo == CAMPO_NAO_ENCONTRADO) {
			if (achouCampoOpcao1 == 2) {
				if (contoursOut[campoDireito][0].x < contoursOut[campoEsquerdo][0].x) {
					swapCampo = campoEsquerdo;
					campoEsquerdo = campoDireito;
					campoDireito = swapCampo;
				}

				Point aEsq, bEsq, cEsq, dEsq, aDir, bDir, cDir, dDir;

				defineABCD(contoursOut[campoEsquerdo], &aEsq, &bEsq, &cEsq, &dEsq); // Seta os pontos A, B, C e D do campo esquerdo.
				defineABCD(contoursOut[campoDireito], &aDir, &bDir, &cDir, &dDir);  // Seta os pontos A, B, C e D do campo direito.	
				defineCantos(aEsq, cEsq, bDir, dDir, &superiorEsq, &inferiorEsq, &superiorDir, &inferiorDir);
			} else if (achouCampoOpcao2 == 2) {
				Point pMedioSuperior, pMedioInferior;
				int diffXSuperior, diffYSuperior, diffXInferior, diffYInferior;
				calculaPontoMedio(contoursOut[limiteSuperior], &pMedioSuperior, &diffXSuperior, &diffYSuperior);
				calculaPontoMedio(contoursOut[limiteInferior], &pMedioInferior, &diffXInferior, &diffYInferior);

				superiorEsq = Point(pMedioSuperior.x - (diffXSuperior / 2), pMedioSuperior.y - (diffYSuperior / 4));
				inferiorEsq = Point(pMedioInferior.x - (diffXInferior / 2), pMedioInferior.y + (diffYInferior / 4));
				superiorDir = Point(pMedioSuperior.x + (diffXSuperior / 2), pMedioSuperior.y - (diffYSuperior / 4));
				inferiorDir = Point(pMedioInferior.x + (diffXInferior / 2), pMedioInferior.y + (diffYInferior / 4));

				superiorEsq.x = fmin(superiorEsq.x, inferiorEsq.x);
				inferiorEsq.x = fmin(superiorEsq.x, inferiorEsq.x);
				superiorDir.x = fmax(superiorDir.x, inferiorDir.x);
				inferiorDir.x = fmax(superiorDir.x, inferiorDir.x);

				superiorEsq.y = fmin(superiorEsq.y, superiorDir.y);
				superiorDir.y = fmin(superiorEsq.y, superiorDir.y);
				inferiorEsq.y = fmax(inferiorEsq.y, inferiorDir.y);
				inferiorDir.y = fmax(inferiorEsq.y, inferiorDir.y);
				// superiorEsq.x = superiorEsq.x - (superiorEsq.x * 0.2);
				// inferiorEsq.x = inferiorEsq.x - (inferiorEsq.x * 0.2);
				// superiorDir.x = superiorDir.x * 1.2;
				// inferiorDir.x = inferiorDir.x * 1.2;
			}
			// imgTransformada = transformaCampo(superiorEsq, inferiorEsq, superiorDir, inferiorDir, imgOriginal);
			statusCampo = CAMPO_ENCONTRADO;
			kernelMorphological = 4;
		}

		if (statusCampo == CAMPO_ENCONTRADO) {
			if (!modoJogo) {
				vector<Point> pts;
				pts.push_back(superiorEsq);
				pts.push_back(inferiorEsq);
				pts.push_back(inferiorDir);
				pts.push_back(superiorDir);
				polylines(imgAnalise, pts, 1, COR_VERDE, 2, 8, 0);
			}

			// Procurar a Bola
			blur(imgBola, imgBola, Size(2*kernelSize+1, 2*kernelSize+1));
			Canny(imgBola, imgBola, threshold, threshold * (2 + thresholdProp), 2 * apertureSize + 1, L2gradient);
			findContours(imgBola, contoursBall, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

			contoursBallOut.resize(contoursBall.size());
			for (size_t k = 0; k < contoursBall.size(); k++ ) {
				approxPolyDP(Mat(contoursBall[k]), contoursBallOut[k], epsilon, true);
				int area = fabs(contourArea(contoursBall[k], 0));
				Point pMedio;
				int diffX, diffY;
				calculaPontoMedio(contoursBallOut[k], &pMedio, &diffX, &diffY);
				if (/*area >= areaBallL && area <= areaBallH &&*/
					pMedio.x >= superiorEsq.x - MARGEM_DE_ERRO_CAMPO && pMedio.x >= inferiorEsq.x - MARGEM_DE_ERRO_CAMPO && 
					pMedio.x <= superiorDir.x + MARGEM_DE_ERRO_CAMPO && pMedio.x <= inferiorDir.x + MARGEM_DE_ERRO_CAMPO &&
					pMedio.y >= superiorEsq.y - MARGEM_DE_ERRO_CAMPO && pMedio.y >= superiorDir.y - MARGEM_DE_ERRO_CAMPO && 
					pMedio.y <= inferiorEsq.y + MARGEM_DE_ERRO_CAMPO && pMedio.y <= inferiorDir.y + MARGEM_DE_ERRO_CAMPO) {

					for (int i = 0; i < TAMANHO_VETOR_JOGADORES; i++) {
						if (posJogadores[i].x != 0 && posJogadores[i].y != 0) {
							int diffX = abs(pMedio.x - posJogadores[i].x);
							int diffY = abs(pMedio.y - posJogadores[i].y);
							if (diffX < 20 && diffY < 20) {
								circle(imgAnalise, pMedio, 20, COR_LARANJA, 3, 8, 0);
								posBola = pMedio;
							}
						}
					}
				}
			}

			// Procurar os jogadores
			defineJogadores(posJogadores);
			if (encontrouJogador3) {
				jogaBonito();
			} else {
				jogaLouco();
			}
			// play(posJogador1[1].x, posJogador1[1].y, grauFrenteJogador1,
			// 	 posJogador2[1].x, posJogador2[1].y, grauFrenteJogador2,
			// 	 posJogador3[1].x, posJogador3[1].y, grauFrenteJogador3,
			// 	 posBola.x, posBola.y,
			// 	 superiorEsq,
			// 	 fmax(superiorDir.x, inferiorDir.x) - fmin(superiorEsq.x, inferiorEsq.x),
			// 	 fmax(inferiorEsq.y, inferiorDir.y) - fmin(superiorEsq.y, superiorDir.y));

			if (!modoJogo) {
				if (encontrouJogador1) {
					circle(imgAnalise, posJogador1[1], 20, COR_VERMELHO, 3, 8, 0);
					line(imgAnalise, posJogador1Aux[0], posJogador1Aux[1], COR_VERMELHO, 2, 8, 0);
				}
				
				if (encontrouJogador2) {
					circle(imgAnalise, posJogador2[1], 20, COR_AZUL, 3, 8, 0);
					line(imgAnalise, posJogador2Aux[0], posJogador2Aux[1], COR_AZUL, 2, 8, 0);
				}

				if (encontrouJogador3) {
					circle(imgAnalise, posJogador3[1], 20, COR_VERDE, 3, 8, 0);
					line(imgAnalise, posJogador3Aux[0], posJogador3Aux[1], COR_VERDE, 2, 8, 0);
				}
			}
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

		if (!modoJogo) {
			resize(imgAnalise, imgAnaliseMenor, Size(0,0), 0.5, 0.5, INTER_LINEAR);
			namedWindow("imgAnalise", WINDOW_AUTOSIZE);
			imshow("imgAnalise", imgAnaliseMenor); //show the thresholded image
		}

	    namedWindow("TESTE", WINDOW_AUTOSIZE);
	    imshow("TESTE", imgTeste);

	    namedWindow("TESTE_ORIGINAL", WINDOW_AUTOSIZE);
	    imshow("TESTE_ORIGINAL", imgOriginal);

		// resize(imgOriginal, imgOriginalMenor, Size(0,0), 0.5, 0.5, INTER_LINEAR);
		// namedWindow("imgOriginal", WINDOW_AUTOSIZE);
		// imshow("imgOriginal", imgOriginal); //show the thresholded image

		// vector<Mat> channelsHSV(3);
		// split(imgHSV, channelsHSV);
	 //    namedWindow("channelsHSV[0]", WINDOW_AUTOSIZE);
	 //    imshow("channelsHSV[0]", channelsHSV[0]);

	 //    namedWindow("channelsHSV[1]", WINDOW_AUTOSIZE);
	 //    imshow("channelsHSV[1]", channelsHSV[1]);

	 //    namedWindow("channelsHSV[2]", WINDOW_AUTOSIZE);
	 //    imshow("channelsHSV[2]", channelsHSV[2]);

		// namedWindow("imgPlayersRGB", WINDOW_NORMAL);
		// imshow( "imgPlayersRGB", imgOriginal);

		// namedWindow("imgPlayersHSV", WINDOW_NORMAL);
		// imshow( "imgPlayersHSV", imgPlayersHSV);

		// imgPlayersRGB.release();
		// imgPlayersHSV.release();
		// mask_image.release();

		saveTrackbarsParameters();

		if (waitKey(30) == 27) { //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
			cout << "Programa encerrado" << endl;
			for (int j = 0; j < 20; j++) {
				sendCommands(1, 0, 0, 0, 0);
				sendCommands(2, 0, 0, 0, 0);
				sendCommands(3, 0, 0, 0, 0);
			}
			break;
		}
	}

	for (int j = 0; j < 20; j++) {
		sendCommands(1, 0, 0, 0, 0);
		sendCommands(2, 0, 0, 0, 0);
		sendCommands(3, 0, 0, 0, 0);
	}
	return 0;
}

/**
+ *     Função: createTrackbarsGlobal
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
	// createTrackbar("vBallL", "Control Ball", &vBallL, 255);
	// createTrackbar("vBallH", "Control Ball", &vBallH, 255);

	// ÁREA DO CAMPO
	// namedWindow("Control Area", WINDOW_NORMAL);
	// createTrackbar("saveAreaParametersOfTrackbar", "Control Area", &saveAreaParametersOfTrackbar, 1);
	// createTrackbar("areaAreaL", "Control Area", &areaAreaL, 40000);
	// createTrackbar("areaAreaH", "Control Area", &areaAreaH, 40000);

	// JOGADORES
	// namedWindow("Control Player", WINDOW_NORMAL);
	// createTrackbar("areaJogadorL", "Control Player", &areaJogadorL, 1500);
	// createTrackbar("areaJogadorH", "Control Player", &areaJogadorH, 1500);

	// JOGADORES AZUL (COR TIME)
	// namedWindow("Control Blue", WINDOW_NORMAL);
	// createTrackbar("saveBlueParametersOfTrackbar", "Control Blue", &saveBlueParametersOfTrackbar, 1);
	// createTrackbar("hBlueL", "Control Blue", &hBlueL, 180);
	// createTrackbar("hBlueH", "Control Blue", &hBlueH, 180);
	// createTrackbar("sBlueL", "Control Blue", &sBlueL, 255);
	// createTrackbar("sBlueH", "Control Blue", &sBlueH, 255);
	// createTrackbar("vBlueL", "Control Blue", &vBlueL, 255);
	// createTrackbar("vBlueH", "Control Blue", &vBlueH, 255);
	// createTrackbar("rBlueL", "Control Blue", &rBlueL, 255);
	// createTrackbar("rBlueH", "Control Blue", &rBlueH, 255);
	// createTrackbar("gBlueL", "Control Blue", &gBlueL, 255);
	// createTrackbar("gBlueH", "Control Blue", &gBlueH, 255);
	// createTrackbar("bBlueL", "Control Blue", &bBlueL, 255);
	// createTrackbar("bBlueH", "Control Blue", &bBlueH, 255);

	// JOGADORES AMARELO (COR TIME)
	// namedWindow("Control Yellow", WINDOW_NORMAL); //create a window called "Control"
	// createTrackbar("saveYellowParametersOfTrackbar", "Control Yellow", &saveYellowParametersOfTrackbar, 1);
	// createTrackbar("hYellowL", "Control Yellow", &hYellowL, 180);
	// createTrackbar("hYellowH", "Control Yellow", &hYellowH, 180);
	// createTrackbar("sYellowL", "Control Yellow", &sYellowL, 255);
	// createTrackbar("sYellowH", "Control Yellow", &sYellowH, 255);
	// createTrackbar("vYellowL", "Control Yellow", &vYellowL, 255);
	// createTrackbar("vYellowH", "Control Yellow", &vYellowH, 255);
	// createTrackbar("rYellowL", "Control Yellow", &rYellowL, 255);
	// createTrackbar("rYellowH", "Control Yellow", &rYellowH, 255);
	// createTrackbar("gYellowL", "Control Yellow", &gYellowL, 255);
	// createTrackbar("gYellowH", "Control Yellow", &gYellowH, 255);
	// createTrackbar("bYellowL", "Control Yellow", &bYellowL, 255);
	// createTrackbar("bYellowH", "Control Yellow", &bYellowH, 255);

	// JOGADORES ROXO (COR IDENTIFICAÇÃO JOGADOR)
	// namedWindow("Control Purple", WINDOW_NORMAL); //create a window called "Control"
	// createTrackbar("savePurpleParametersOfTrackbar", "Control Purple", &savePurpleParametersOfTrackbar, 1);
	// createTrackbar("hPurpleL", "Control Purple", &hPurpleL, 180);
	// createTrackbar("hPurpleH", "Control Purple", &hPurpleH, 180);
	// createTrackbar("sPurpleL", "Control Purple", &sPurpleL, 255);
	// createTrackbar("sPurpleH", "Control Purple", &sPurpleH, 255);
	// createTrackbar("vPurpleL", "Control Purple", &vPurpleL, 255);
	// createTrackbar("vPurpleH", "Control Purple", &vPurpleH, 255);

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

	// JOGADORES GERAL
	namedWindow("Control Jogadores", WINDOW_NORMAL);
	createTrackbar("saveJogadoresParametersOfTrackbar", "Control Jogadores", &saveJogadoresParametersOfTrackbar, 1);
	createTrackbar("areaJogadorL", "Control Jogadores", &areaJogadorL, 700);
	createTrackbar("areaJogadorH", "Control Jogadores", &areaJogadorH, 700);
	createTrackbar("areaId1L", "Control Jogadores", &areaId1L, 500);
	createTrackbar("areaId1H", "Control Jogadores", &areaId1H, 500);
	createTrackbar("areaId2L", "Control Jogadores", &areaId2L, 500);
	createTrackbar("areaId2H", "Control Jogadores", &areaId2H, 500);
	createTrackbar("areaId3L", "Control Jogadores", &areaId3L, 800);
	createTrackbar("areaId3H", "Control Jogadores", &areaId3H, 800);
}

void saveTrackbarsParameters() {
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
		ballParametersNewFile << "hL=" << hBallL << endl;
		ballParametersNewFile << "hH=" << hBallH << endl;
		ballParametersNewFile << "sL=" << sBallL << endl;
		ballParametersNewFile << "sH=" << sBallH << endl;
		ballParametersNewFile << "vL=" << vBallL << endl;
		ballParametersNewFile << "vH=" << vBallH << endl;
		ballParametersNewFile.close();
		saveBallParametersOfTrackbar = FALSE;
	}

	if (savePurpleParametersOfTrackbar) {
		ofstream purpleParametersNewFile;
		purpleParametersNewFile.open("parameters/purple-color-parameters.txt");
		purpleParametersNewFile << "hL=" << hPurpleL << endl;
		purpleParametersNewFile << "hH=" << hPurpleH << endl;
		purpleParametersNewFile << "sL=" << sPurpleL << endl;
		purpleParametersNewFile << "sH=" << sPurpleH << endl;
		purpleParametersNewFile << "vL=" << vPurpleL << endl;
		purpleParametersNewFile << "vH=" << vPurpleH << endl;
		purpleParametersNewFile.close();
		savePurpleParametersOfTrackbar = FALSE;
	}

	if (saveAreaParametersOfTrackbar) {
		ofstream areaParametersNewFile;
		areaParametersNewFile.open("parameters/area-parameters.txt");
		areaParametersNewFile << "areaL=" << areaAreaL << endl;
		areaParametersNewFile << "areaH=" << areaAreaH << endl;
		areaParametersNewFile.close();
		saveAreaParametersOfTrackbar = FALSE;
	}

	if (saveJogadoresParametersOfTrackbar) {
		ofstream jogadoresParametersNewFile;
		jogadoresParametersNewFile.open("parameters/jogadores-parameters.txt");
		jogadoresParametersNewFile << "areaJogadorL=" << areaJogadorL << endl;
		jogadoresParametersNewFile << "areaJogadorH=" << areaJogadorH << endl;
		jogadoresParametersNewFile << "areaId1L=" << areaId1L << endl;
		jogadoresParametersNewFile << "areaId1H=" << areaId1H << endl;
		jogadoresParametersNewFile << "areaId2L=" << areaId2L << endl;
		jogadoresParametersNewFile << "areaId2H=" << areaId2H << endl;
		jogadoresParametersNewFile << "areaId3L=" << areaId3L << endl;
		jogadoresParametersNewFile << "areaId3H=" << areaId3H << endl;
		jogadoresParametersNewFile.close();
		saveJogadoresParametersOfTrackbar = FALSE;
	}
}

void sendCommands(int robot, int dR, int sR, int dL, int sL) {
	char directionRight[3], speedRight[4], directionLeft[3], speedLeft[4];
	sprintf(directionRight, "%d", dR);
	sprintf(directionLeft, "%d", dL);
	sprintf(speedRight, "%d", sR);
	sprintf(speedLeft, "%d", sL);

	const int buf_max = 256;
	char buf[buf_max];

	if (!openConnection) {
		char serialPort[] = "/dev/tty.usbmodem1411";
		fd = serialport_init(serialPort, 9600);
		if (fd == -1) {
			cout << "couldn't open port" << endl;
		} else {
			openConnection = TRUE;
		}
	}

	char data[18] = {'S', (char) (robot + 48)};
	strcat(data, "#");
	strcat(data, directionRight);
	strcat(data, "#");
	strcat(data, speedRight);
	strcat(data, "#");
	strcat(data, directionLeft);
	strcat(data, "#");
	strcat(data, speedLeft);
	strcat(data, "E\0");

	cout << data << endl;
	sprintf(buf, "%s", data);
	int rc = serialport_write(fd, buf);
	if (rc == -1) cout << "error writing" << endl;
}

void defineJogadores(Point posJogadores[]) {
	int menorDiffX, menorDiffY, melhorPos;

	encontrouJogador1 = false;
	encontrouJogador2 = false;
	encontrouJogador3 = false;

	for (int i = 0; i < 3; i++) {
		menorDiffX = menorDiffY = 50;
		melhorPos = -1;
		for (int j = 0; j < TAMANHO_VETOR_JOGADORES; j++) {
			if (posJogadores[j].x != 0 && posJogadores[j].y != 0) {
				if (i == 0 && posJogador1[0].x != 0 && posJogador1[0].y != 0) {
					int diffX = abs(posJogador1[0].x - posJogadores[j].x);
					int diffY = abs(posJogador1[0].y - posJogadores[j].y);
					if (diffX < menorDiffX && diffY < menorDiffY) {
						melhorPos = j;
						menorDiffX = diffX;
						menorDiffY = diffY;
					}
				} else if (i == 1 && posJogador2[0].x != 0 && posJogador2[0].y != 0) {
					int diffX = abs(posJogador2[0].x - posJogadores[j].x);
					int diffY = abs(posJogador2[0].y - posJogadores[j].y);
					if (diffX < menorDiffX && diffY < menorDiffY) {
						melhorPos = j;
						menorDiffX = diffX;
						menorDiffY = diffY;
					}
				} else if (i == 2 && posJogador3[0].x != 0 && posJogador1[0].y != 0) {
					int diffX = abs(posJogador3[0].x - posJogadores[j].x);
					int diffY = abs(posJogador3[0].y - posJogadores[j].y);
					if (diffX < menorDiffX && diffY < menorDiffY) {
						melhorPos = j;
						menorDiffX = diffX;
						menorDiffY = diffY;
					}
				}
			}
		}
		if (i == 0 && melhorPos != -1)
			posJogador1[1] = posJogadores[melhorPos];
		if (i == 1 && melhorPos != -1)
			posJogador2[1] = posJogadores[melhorPos];
		if (i == 2 && melhorPos != -1)
			posJogador3[1] = posJogadores[melhorPos];
	}

	if (posJogador1[0].x != 0 && posJogador1[0].y != 0 && posJogador1[1].x != 0 && posJogador1[1].y != 0) {
		encontrouJogador1 = true;
		defineFrenteJogador(1);
	}
	if (posJogador2[0].x != 0 && posJogador2[0].y != 0 && posJogador2[1].x != 0 && posJogador2[1].y != 0) {
		encontrouJogador2 = true;
		defineFrenteJogador(2);
	}
	if (posJogador3[0].x != 0 && posJogador3[0].y != 0 && posJogador3[1].x != 0 && posJogador3[1].y != 0) {
		encontrouJogador3 = true;
		defineFrenteJogador(3);
	}
}

void defineFrenteJogador(int jogador) {
	if (jogador == 1) {
		float graus;
		// a = coeficiente angular da reta perpendicular a reta que liga os dois pontos do Robo 1
		float a = (float) (posJogador1[1].x - posJogador1[0].x) / (float) (posJogador1[0].y - posJogador1[1].y);
		int x = (posJogador1[0].x + posJogador1[1].x) / 2;
		int y = (posJogador1[0].y + posJogador1[1].y) / 2;
		float b = y - (a * x);
		int yCheck = (a * posJogador1[1].x) + b;

		posJogador1Aux[0] = Point(x, y);
		posJogador1Aux[1] = Point(posJogador1[1].x, yCheck);

		if (yCheck == posJogador1[1].y) {
			if (posJogador1[1].x < x) {
				graus = 90;
			} else {
				graus = 270;
			}
		} else if (yCheck > posJogador1[1].y) {
			graus = atan(a) * 180 / PI + 180;
		} else {
			graus = atan(a) * 180 / PI;
			if (graus < 0) graus = graus + 360;

		}
		// cout << "graus1: " << graus << endl;
		grauFrenteJogador1 = graus;
	} else if (jogador == 2) {
		float graus;
		float a = (float) (posJogador2[1].x - posJogador2[0].x) / (float) (posJogador2[0].y - posJogador2[1].y);
		float teta = atan(0.4)*180/PI;
		a = -(1.0f/(tan(atan(a)*180/PI+atan(0.4)*180/PI)));

		int x = (posJogador2[0].x + posJogador2[1].x) / 2;
		int y = (posJogador2[0].y + posJogador2[1].y) / 2;
		float b = y - (a * x);
		int yCheck = (a * posJogador2[1].x) + b;

		posJogador2Aux[0] = Point(x, y);
		posJogador2Aux[1] = Point(x * 2, (a * x * 2) + b);

		if (yCheck == posJogador2[1].y) {
			if (posJogador2[1].x < x) {
				graus = 90;
			} else {
				graus = 270;
			}
		} else if (yCheck > posJogador2[1].y) {
			graus = atan(a)*180/PI + 180;
		} else {
			graus = atan(a)*180/PI;
			if(graus<0) graus = graus+360;

		}
		// cout << "graus2: " << graus << endl;
		grauFrenteJogador2 = graus;
	} else if (jogador == 3) {
		float graus;
		float a = (float) (posJogador3[1].x - posJogador3[0].x) / (float) (posJogador3[0].y - posJogador3[1].y);
		float teta = atan(0.4)*180/PI;
		a = -(1.0f/(tan(atan(a)*180/PI+atan(0.875)*180/PI)));

		int x = (posJogador3[0].x + posJogador3[1].x) / 2;
		int y = (posJogador3[0].y + posJogador3[1].y) / 2;
		float b = y - (a * x);
		int yCheck = (a * posJogador3[1].x) + b;

		posJogador3Aux[0] = Point(x, y);
		posJogador3Aux[1] = Point(x * 2, (a * x * 2) + b);

		if (yCheck == posJogador3[1].y) {
			if (posJogador3[1].x < x) {
				graus = 90;
			} else {
				graus = 270;
			}
		} else if (yCheck > posJogador3[1].y) {
			graus = atan(a)*180/PI + 180;
		} else {
			graus = atan(a)*180/PI;
			if(graus<0) graus = graus+360;

		}
		// cout << "graus3: " << graus << endl;
		grauFrenteJogador3 = graus;
	}
}

void jogaBonito() {
	float a = (float) (posJogador1[1].x - posJogador1[0].x) / (float) (posJogador1[0].y - posJogador1[1].y);
	int x = (posJogador1[0].x + posJogador1[1].x) / 2;
	int y = (posJogador1[0].y + posJogador1[1].y) / 2;
	float b = y - (a * x);

	float yCheck = (a * posBola.x) + b;
	if (yCheck > posBola.y - 20 && yCheck < posBola.y + 20 &&
		((posJogador3[1].x > posJogador3[0].x && posJogador3[1].x < posBola.x) ||
		 (posJogador3[1].x < posJogador3[0].x && posJogador3[1].x > posBola.x))) {
		sendCommands(3, 1, 300, 1, 300);
	} else {
		sendCommands(3, 1, 200, -1, 200);
	}
}

void jogaLouco() {
	int aleatorio1 = rand();
	int aleatorio2 = rand();

	int directionRight = (aleatorio1 % 9 == 0 ? 1 : -1);
	int directionLeft = (aleatorio2 % 9 == 0 ? 1 : -1);
	int speedRight = 275 + (aleatorio1 % 3 == 0 && aleatorio1 + 275 < 1000 ? aleatorio1 : 0);
	int speedLeft = 275 + (aleatorio2 % 3 == 0 && aleatorio2 + 275 < 1000 ? aleatorio2 : 0);
	sendCommands(3, directionRight, speedRight, directionLeft, speedLeft);
	// sleep(3000);
}
