

////////////////////////////////Cabeceras/////////////////////////////////////
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
#include <numbers>

/////////////////////////////////////////////////////////////////////////////

///////////////////////////////Espacio de nombres////////////////////////////
using namespace cv;
using namespace std;
//////////////////////////////Funciones previas///////////////////////////////////////////////

float** crear_kernel(int dimension, float sigma) { 


	int indice = 0;

	float** kernel = new float* [dimension];
	indice = ceil(dimension / 2);
	if (dimension % 2 == 0) {
		cout << "\nsolo se aceptan numeros impares";
		crear_kernel(dimension, sigma);
	}
	else {

		for (int i = 0; i < dimension; i++)
		{
			kernel[i] = new float[dimension];
		}
		for (int i = 0; i < dimension; i++) {
			for (int j = 0; j < dimension; j++)
			{
				kernel[i][j] = 0;

			}
		}

		for (int i = -indice; i <= indice; i++) {
			for (int j = indice; j >= -indice; j--)
			{
				kernel[-(j - indice)][i + indice] = 1 / (2 * 3.1416 * pow(sigma, 2)) * pow(2.71828, -((pow(i, 2) + pow(j, 2)) / 2 * sigma));
				
			}
			
		}
	}
	return kernel;
}

/////////////////////////Inicio de la funcion principal///////////////////
int main()
{

	/********Declaracion de variables generales*********/
	char NombreImagen[] = "C:\\Users\\youtu\\Documents\\ESCOM\\Quinto semestre\\Vision artificial\\1.- Parcial primero\\Tarea 1 opencv\\Tarea entorno openCV\\lena.jpg";
	Mat imagen; // Matriz que contiene nuestra imagen sin importar el formato
	/************************/

	/*********Lectura de la imagen*********/
	imagen = imread(NombreImagen);

	if (!imagen.data)
	{
		cout << "Error al cargar la imagen: " << NombreImagen << endl;
		exit(1);
	}
	/************************/

	/************Procesos*********/
	int fila_original = imagen.rows;
	int columna_original = imagen.cols;//Lectur de cuantas columnas
	Mat imagenGris(fila_original, columna_original, CV_8UC1);
	cvtColor(imagen, imagenGris, COLOR_BGR2GRAY); //pasar imagen a gris 
	
	//crear kernel
	int dimension = 0;
	float sigma;
	int indice = 0;
	cout << "\nescriba la dimension del kernel por favor (solo se aceptan valores impares )";
	cin >> dimension;
	cout << "escriba el valor de sigma  ";
	cin >> sigma;

	indice = ceil(dimension / 2);
	float** kernel = crear_kernel(dimension, sigma);

	//imprimir kernel
	cout << "kernel: \n";
	for (int i = 0; i < dimension; i++) {
		for (int j = 0; j < dimension; j++)
		{
			cout << kernel[i][j];
			cout << " ";
		}
		cout << "\n";
	}

	// crear imagenes nuevas 
	int nuevas_columas = columna_original + indice*2;
	int nuevas_filas = fila_original + indice*2;
	Mat imagenConBordes(nuevas_filas, nuevas_columas, CV_8UC1);//imagen con bordes
	Mat imagenGauss(fila_original, columna_original, CV_8UC1);//imagen suavisada 
	Mat imagenEcualizada(fila_original, columna_original, CV_8UC1);//imagen ecualizada
	Mat imagenAuxiliar(fila_original+2, columna_original+2, CV_8UC1);//para aplicar kernel de gradiantes

	for (int i = 0; i < nuevas_columas; i++) {
		for (int j = 0; j < nuevas_filas; j++)
		{
			imagenConBordes.at<uchar>(Point(i, j)) = 0;//rellenamos la imagen con bordes con ceros 
		}
	}
	//copiamos la imagen en la imagen con bordes
	for (int i = 0; i < columna_original; i++) {
		for (int j = 0; j < fila_original; j++)
		{
			imagenConBordes.at<uchar>(Point(i + indice, j + indice)) = imagenGris.at<uchar>(Point(i, j));
		}
	}
	//aplicamos suavizado con gauss 

	for (int i = indice; i < columna_original+indice; i++) {
		for (int j = indice; j < fila_original+indice; j++)
		{
			float suma_auxiliar = 0;
			for (int k = 0; k < dimension; k++)
			{
				for (int l = 0; l < dimension; l++)
				{
					suma_auxiliar = suma_auxiliar + kernel[k][l] * imagenConBordes.at<uchar>(Point(i - indice + k, j - indice + l));

				}

			}

			imagenGauss.at<uchar>(Point(i - indice, j - indice)) = ceil(suma_auxiliar);
		}
	}
	//ecualizamos imagen
	equalizeHist(imagenGauss, imagenEcualizada);
	
	// obtener gradiantes
	//llenamos de ceros imagen auxiliar para kernel 3x3
	for (int i = 0; i < columna_original+2; i++) {
		for (int j = 0; j < fila_original + 2; j++)
		{
			imagenAuxiliar.at<uchar>(Point(i, j)) = 0;//rellenamos la imagen con bordes con ceros 
		}
	}
	//copiamos la imagen en la imagen con bordes
	for (int i = 0; i < columna_original; i++) {
		for (int j = 0; j < fila_original; j++)
		{
			imagenAuxiliar.at<uchar>(Point(i + 1, j + 1)) = imagenEcualizada.at<uchar>(Point(i, j));
		}
	}
	float hx[3][3] = { {-1,0,1},{-2,0,2},{-1,0,1} };
	float hy[3][3] = { {1,2,1},{0,0,0},{-1,-2,-1} };
	Mat imagenhx(fila_original, columna_original, CV_8UC1);//imagen hx
	Mat imagenhy(fila_original, columna_original, CV_8UC1);//imagen hy
	//recorremos y guardamos imagenes
	for (int i = 1; i < columna_original + 1; i++) {
		for (int j = 1; j < fila_original + 1; j++)
		{
			float suma_auxiliar_hx = 0;
			float suma_auxiliar_hy = 0;
			for (int k = 0; k < 3; k++)
			{
				for (int l = 0; l < 3; l++)
				{
					suma_auxiliar_hx = suma_auxiliar_hx + (hx[k][l] * imagenAuxiliar.at<uchar>(Point(i - 1 + k, j - 1 + l)));
					suma_auxiliar_hy = suma_auxiliar_hy + (hy[k][l] * imagenAuxiliar.at<uchar>(Point(i - 1 + k, j - 1 + l)));
				}
			}
			imagenhx.at<uchar>(Point(i - 1, j - 1)) = ceil(suma_auxiliar_hx);
			imagenhy.at<uchar>(Point(i - 1, j - 1)) = ceil(suma_auxiliar_hy);
		}
	}

	//obtenemos imagen g
	Mat imagenG(fila_original, columna_original, CV_8UC1);//imagen hx
	for (int i = 0; i < columna_original ; i++) {
		for (int j = 0; j < fila_original ; j++)
		{
			imagenG.at<uchar>(Point(i , j)) = ceil(sqrt(pow( imagenhx.at<uchar>(Point(i , j )),2)+ pow(imagenhy.at<uchar>(Point(i, j)), 2)));
		}
	}

	//obtenemos gradiantes
	Mat imagenGradiante(fila_original, columna_original, CV_8UC1);//imagen gradiante
	for (int i = 0; i < columna_original; i++) {
		for (int j = 0; j < fila_original; j++)
		{
			imagenGradiante.at<uchar>(Point(i, j)) = ceil(atan( imagenhy.at<uchar>(Point(i, j))/( imagenhx.at<uchar>(Point(i, j))+1)));
		}
	}

	//establecer maximos y minimos
	int max = 229,min=89;
	Mat imagenAcotada(fila_original, columna_original, CV_8UC1);//imagen acotada
	//recorremos toda  la imagen g
	for (int i = 0; i < columna_original; i++) {
		for (int j = 0; j < fila_original; j++)
		{
			if (imagenG.at<uchar>(Point(i, j))<=min) {
				imagenAcotada.at<uchar>(Point(i, j)) = 0;
			}else 
			if (imagenG.at<uchar>(Point(i, j)) >= max) {
				imagenAcotada.at<uchar>(Point(i, j)) = 255;
			}
			else {
				imagenAcotada.at<uchar>(Point(i, j)) = imagenG.at<uchar>(Point(i, j));
			}
		}
	}

	namedWindow("imagen_original", WINDOW_AUTOSIZE);//imagen original
	imshow("imagen_original", imagen);
	cout << "\nTamaño imagen1: (" << imagen.cols << "," << imagen.rows << ") \n";

	namedWindow("imagen_gris", WINDOW_AUTOSIZE);//imagen con bordes
	imshow("imagen_gris", imagenGris);
	cout << "\nTamaño imagen2: (" << imagenGris.cols << "," << imagenGris.rows << ") \n";

	//namedWindow("imagen_con_bordes", WINDOW_AUTOSIZE);//imagen con bordes
	//imshow("imagen_con_borde", imagenConBordes);

	imshow("imagen_suavizada_(gauss)", imagenGauss);//imagen suavizada 
	cout << "\nTamaño imagen3: (" << imagenGauss.cols << "," << imagenGauss.rows << ") \n";

	imshow("imagen_ecualizada", imagenEcualizada );//imagen ecualizada
	cout << "\nTamaño imagen4: (" << imagenEcualizada.cols << "," << imagenEcualizada.rows << ") \n";

	//imshow("imagen_hx", imagenhx);//imagen derivada hx
	//imshow("imagen_hy", imagenhy);//imagen derivada hy

	imshow("imagen_G", imagenG);//imagen |G|
	cout << "\nTamaño imagen5: (" << imagenG.cols << "," << imagenG.rows << ") \n";


	imshow("borde_canny", imagenAcotada);//imagen canny (acotada)
	cout << "\nTamaño imagen6: (" << imagenAcotada.cols << "," << imagenAcotada.rows << ") \n";
	/************************/

	waitKey(0); //Función para esperar
	return 1;
	//fin
}
/////////////////////////////////////////////////////////////////////////