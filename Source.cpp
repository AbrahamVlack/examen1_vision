#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "cmath"
#include "vector"
#include "iostream"
#define e 2.72
#define pi 3.14


using namespace std;
using namespace cv;


vector<vector<float>> funkernel(int *, int *);
float filtro_al_pixel(Mat , vector<vector<float>> , int , int , int );
Mat filtro_a_matriz(Mat, vector<vector<float>>, int);



void ecualizacion(Mat, Mat*);
void filtro_sobel(Mat, Mat*, Mat*, Mat*);
float convolucion(Mat, vector<vector<float>>, int, int, int);
vector<vector<float>> kernel(vector <int>);


int main()
{
	int sigma, tamker;
	Mat img; 

	img = imread("C:/Users/Abraham/Pictures/lena.png"); // obtencion de la imagen

	if (!img.data)
	{
		cout << "Error al cargar la imagen: "<< "Lena" <<endl; // en caso de no poder abrir la imagen 
		exit(1);
	}

	int fila_original = img.rows;
	int columna_original = img.cols;


	Mat ntsc(fila_original, columna_original, CV_8UC1);

	for (int i = 0; i < fila_original; i++) //transformacion de la imagen a ntsc
	{
		for (int j = 0; j < columna_original; j++)
		{
			double azul = img.at<Vec3b>(Point(j, i)).val[0];  
			double verde = img.at<Vec3b>(Point(j, i)).val[1]; 
			double rojo = img.at<Vec3b>(Point(j, i)).val[2]; 

			ntsc.at<uchar>(Point(j, i)) = uchar(0.299 * rojo + 0.587 * verde + 0.114 * azul);
		}
	}

	vector<vector<float>> kernel = funkernel(&tamker, &sigma);
	Mat filtrada = filtro_a_matriz(ntsc, kernel, tamker);

	Mat ecualizada(filtrada.rows, filtrada.cols, CV_8UC1);
	ecualizacion(filtrada, &ecualizada);

	Mat vertical(ecualizada.rows, ecualizada.cols, CV_8UC1);
	Mat horizontal(ecualizada.rows, ecualizada.cols, CV_8UC1);
	Mat sobel(ecualizada.rows, ecualizada.cols, CV_8UC1);

	filtro_sobel(ecualizada, &vertical, &horizontal, &sobel); // le pasamos a la funcion
	// la matriz ya escualizada y 3 apuntadores 2 para los bordes el cual es el vertical y horizontal
	// y recuperamos el apuntador de la matriz sobel ya modificada 


	// Mostrar imagenes
	namedWindow("gris", WINDOW_AUTOSIZE);
	imshow("gris", ntsc);

	namedWindow("filtro", WINDOW_AUTOSIZE);
	imshow("filtro", filtrada);

	namedWindow("Ecualizada", WINDOW_AUTOSIZE);
	imshow("Ecualizada", ecualizada);

	namedWindow("Filtro sobel", WINDOW_AUTOSIZE);
	imshow("Filtro sobel", sobel);
	waitKey(0);
	return 0;
}

vector<vector<float>> funkernel(int *tamkernel, int *sigma) {

	cout << "Sigma:"; cin >> *sigma;
	cout << "Kernel:"; cin >> *tamkernel;
	if (*tamkernel % 2 == 0) {
		cout << "Valor invalido" << endl;
		exit(1);
	}
	
	int amountSlide = (*tamkernel - 1) / 2;
	vector<vector<float>> vec(*tamkernel, vector<float>(*tamkernel, 0));

	for (int i = -amountSlide; i <= amountSlide; i++)
	{
		for (int j = -amountSlide; j <= amountSlide; j++)
		{
			float resultado = (1 / (2 * pi * (*sigma) * (*sigma))) * pow(e, -((i * i + j * j) / (2 * (*sigma) * (*sigma))));
			vec[i + amountSlide][j + amountSlide] = resultado;
			//cout << resultado << endl;
		}
	}
	return vec;
}

float filtro_al_pixel(Mat original, vector<vector<float>> kernel, int tamkernel, int x, int y) {
	int rows = original.rows;
	int cols = original.cols;
	int amountSlide = (tamkernel - 1) / 2;
	float sumFilter = 0;
	float sumKernel = 0;
	for (int i = -amountSlide; i <= amountSlide; i++)
	{
		for (int j = -amountSlide; j <= amountSlide; j++)
		{
			float kTmp = kernel[i + amountSlide][j + amountSlide];
			int tmpX = x + i;
			int tmpY = y + j;
			float tmp = 0;
			if (!(tmpX < 0 || tmpX >= cols || tmpY < 0 || tmpY >= rows)) {
				tmp = original.at<uchar>(Point(tmpX, tmpY));
			}

			sumFilter += (kTmp * tmp);
			sumKernel += kTmp;
		}
	}
	return (sumFilter/sumKernel);
}

Mat filtro_a_matriz(Mat original, vector<vector<float>> kernel, int tamkernel) {
	Mat filteredImg(original.rows, original.cols, CV_8UC1);
	for (int i = 0; i < original.rows; i++)
	{
		for (int j = 0; j < original.cols; j++) {
			filteredImg.at<uchar>(Point(i, j)) = uchar(filtro_al_pixel(original, kernel, tamkernel, i, j));
		}
	}
	return filteredImg;
}


void ecualizacion(Mat imagen, Mat* ecualizada) {

	vector <int> vec_histogram(256); // Vector para guardar los valores del histograma

	
	for (int i = 0; i < imagen.rows; i++) { // Recorrido de cada pixel de la imagen adicionan 1
		for (int j = 0; j < imagen.cols; j++) {
			vec_histogram[imagen.at<uchar>(Point(j, i))] += 1;
		}
	}
		 
	vector <float> frec(256, 0); // calculo de la frecuencia
	for (int i = 0; i < vec_histogram.size(); i++) {
		frec[i] = ((float)(vec_histogram[i])) / ((float)(imagen.rows * imagen.cols));
	}

	
	vector <int> lev_gris(256); //nuevos niveles de gris usando la frec
	float sum = 0;
	for (int i = 0; i < frec.size(); i++) {
		sum = sum+frec[i];
		lev_gris[i] = round((255) * (sum));
	}

	for (int i = 0; i < imagen.rows; i++) { // se pasa por apuntador matriz de la imagen en la que se colocan los nuevos valores de 
		// la imagen ya ecualizada
		for (int j = 0; j < imagen.cols; j++) {
			(* ecualizada).at<uchar>(Point(j, i)) = uchar(lev_gris[imagen.at<uchar>(Point(j, i))]);
		}
	}
}


vector<vector<float>> kernel(vector <int> vec) {

	vector<vector<float>> kernel(3, vector<float>(3, 0));
	
	int contador = 0;
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			kernel[i][j] = vec[contador];
			contador += 1;
		}
	}
	
	return kernel;
}

void filtro_sobel(Mat ecualizada, Mat* sobelVertical, Mat* sobelHorizontal, Mat* sobel) {

	
	vector <int> vertical{ -1, 0, 1, -2, 0, 2, -1, 0, 1 }; // mascara del filtro vertical
	vector<vector<float>> kernelVertical = kernel(vertical);
	for (int i = 0; i < ecualizada.rows; i++) {
		for (int j = 0; j < ecualizada.cols; j++) {
			(*sobelVertical).at<uchar>(Point(j, i)) = uchar(convolucion(ecualizada, kernelVertical, 3, j, i)); //Se asigna el valor sobel a cada pixel de la mat
		}
	}
	
	vector <int> horizontal{ -1, -2, -1, 0, 0, 0, 1, 2, 1 }; // mascara del filtro horizontal
	vector<vector<float>> kernelHorizontal = kernel(horizontal);
	for (int i = 0; i < ecualizada.rows; i++) {
		for (int j = 0; j < ecualizada.cols; j++) {
			(*sobelHorizontal).at<uchar>(Point(j, i)) = uchar(convolucion(ecualizada, kernelHorizontal, 3, j, i));
		}
	}

	
	for (int i = 0; i < ecualizada.rows; i++) { //Uso de |G|
		for (int j = 0; j < ecualizada.cols; j++) {
			(*sobel).at<uchar>(Point(j, i)) = round(sqrt((pow((*sobelHorizontal).at<uchar>(Point(j, i)), 2) + pow((*sobelVertical).at<uchar>(Point(j, i)), 2))));
		}
	}

}

float convolucion(Mat matriz, vector<vector<float>> kernel, int kernel_size, int x, int y) {
	float total = 0;
	int jump = (kernel_size - 1) / 2; //salto

	for (int i = -jump; i <= jump; i++) {
		for (int j = -jump; j <= jump; j++) {

			float kerneltemp = kernel[i + jump][j + jump];
			int tempx = x + i;
			int tempy = y + j;
			float temp = 0;

			if (!(tempx < 0 || tempx >= matriz.cols || tempy < 0 || tempy >= matriz.rows)) {
				temp = matriz.at<uchar>(Point(tempx, tempy));
			}

			total += (kerneltemp * temp);
		}
	}

	//retorno del valor
	if (total > 255) {
		 return total = 255;

	}
	else if (total < 0) {
		return total = 0;
	}
	else {
		return total;
	}

}