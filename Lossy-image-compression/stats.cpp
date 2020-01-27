
#include "stats.h"

stats::stats(PNG & im){
// intiliazing private lists

double prevSat = 0;
double prevLum = 0;
double prevHueX = 0;
double prevHueY = 0;
int onesize = 0;
//initiliaze sumSat
sumSat.resize(im.width());
sumLum.resize(im.width());
sumHueX.resize(im.width());
sumHueY.resize(im.width());
hist.resize(im.width());
for (int i = 0; i < im.width(); i++) {
    sumSat[i].resize(im.height());
    sumLum[i].resize(im.height());
    sumHueX[i].resize(im.height());
    sumHueY[i].resize(im.height());
    hist[i].resize(im.height());
    for (int j = 0; j < im.height(); j++) {
        hist[i][j].resize(36);
        HSLAPixel * curr = im.getPixel(i, j);
        int hue = (int) atan2(sin(curr->h * PI/180), cos(curr->h * PI/180)) * 180/PI;
            if (hue < 0) {
                hue = hue + 360;
            }


    
        if (i== 0 && j== 0) {
            HSLAPixel * curr = im.getPixel(i, j);
            sumSat[i][j] = curr->s; 
            sumLum[i][j] = curr->l; 
            sumHueX[i][j] = cos(curr->h*PI/180); 
            sumHueY[i][j] = sin(curr->h*PI/180); 
            hist[i][j][hue/10]++;

        }
        else if (i==0) {
            HSLAPixel * curr = im.getPixel(i,j);
            sumSat[i][j] = sumSat[i][j-1] + curr->s;
            sumLum[i][j] = sumLum[i][j-1] + curr->l;
            sumHueX[i][j] = sumHueX[i][j-1] + cos(curr->h*PI/180);
            sumHueY[i][j] = sumHueY[i][j-1] + sin(curr->h*PI/180);

            hist[i][j]=hist[i][j-1];
            hist[i][j][hue/10]++;
        }
        else if (j==0) {
            HSLAPixel * curr = im.getPixel(i,j);
            sumSat[i][j] = sumSat[i-1][j] + curr->s;
            sumLum[i][j] = sumLum[i-1][j] + curr->l;
            sumHueX[i][j] = sumHueX[i-1][j] + cos(curr->h*PI/180);
            sumHueY[i][j] = sumHueY[i-1][j] + sin(curr->h*PI/180);
            hist[i][j]=hist[i-1][j];
            hist[i][j][hue/10]++;
        }
        else {
            HSLAPixel * curr = im.getPixel(i,j);
            sumSat[i][j] = sumSat[i-1][j] + sumSat[i][j-1] - sumSat[i-1][j-1] + curr->s;
            sumLum[i][j] = sumLum[i-1][j] + sumLum[i][j-1] - sumLum[i-1][j-1] + curr->l;
            sumHueX[i][j] = sumHueX[i-1][j] + sumHueX[i][j-1] - sumHueX[i-1][j-1] + cos(curr->h*PI/180);
            sumHueY[i][j] = sumHueY[i-1][j] + sumHueY[i][j-1] - sumHueY[i-1][j-1] + sin(curr->h*PI/180);
            for (int k = 0; k < 36; k++) {
                hist[i][j][k] = hist[i-1][j][k] + hist[i][j-1][k] - hist[i-1][j-1][k];
            }
            hist[i][j][hue/10]++;
        }

    }
}


}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
long width = (long) (lr.first - ul.first)+1;
long height = (long) (lr.second - ul.second)+1;


return width*height;
}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){

HSLAPixel retVal = HSLAPixel();

if ((ul.first==0) && (ul.second == 0)) {
    retVal.h = (atan2((sumHueY[lr.first][lr.second])/rectArea(ul, lr), (sumHueX[lr.first][lr.second])/rectArea(ul, lr)) * 180/PI);
    retVal.s = (sumSat[lr.first][lr.second])/rectArea(ul, lr);
    retVal.l = (sumLum[lr.first][lr.second])/rectArea(ul, lr);
    retVal.a = 1.0;
}
else if (ul.first == 0) {
retVal.h  = (atan2((sumHueY[lr.first][lr.second] - sumHueY[lr.first][ul.second-1])/rectArea(ul, lr) ,  (sumHueX[lr.first][lr.second] - sumHueX[lr.first][ul.second-1])/rectArea(ul, lr)) * 180/PI);
retVal.s = (sumSat[lr.first][lr.second] - sumSat[lr.first][ul.second-1])/rectArea(ul, lr);
retVal.l = (sumLum[lr.first][lr.second] - sumLum[lr.first][ul.second-1])/rectArea(ul, lr);
retVal.a = 1.0;
}
else if (ul.second == 0) {
retVal.h  = (atan2((sumHueY[lr.first][lr.second] - sumHueY[ul.first-1][lr.second])/rectArea(ul,lr),  (sumHueX[lr.first][lr.second] - sumHueX[ul.first-1][lr.second])/rectArea(ul, lr)) * 180/PI);
retVal.s = (sumSat[lr.first][lr.second] - sumSat[ul.first-1][lr.second])/rectArea(ul, lr);
retVal.l = (sumLum[lr.first][lr.second] - sumLum[ul.first-1][lr.second])/rectArea(ul, lr);
retVal.a = 1.0;
}
else {
retVal.h  = (atan2((sumHueY[lr.first][lr.second] - sumHueY[ul.first-1][lr.second] - (sumHueY[lr.first][ul.second-1] - sumHueY[ul.first-1][ul.second-1]))/rectArea(ul, lr),  (sumHueX[lr.first][lr.second] - sumHueX[ul.first-1][lr.second] - (sumHueX[lr.first][ul.second-1] - sumHueX[ul.first-1][ul.second-1]))/rectArea(ul, lr)) * 180/PI);
retVal.s = (sumSat[lr.first][lr.second] - sumSat[ul.first-1][lr.second] - (sumSat[lr.first][ul.second-1] - sumSat[ul.first-1][ul.second-1]))/rectArea(ul, lr);
retVal.l = (sumLum[lr.first][lr.second] - sumLum[ul.first-1][lr.second] - (sumLum[lr.first][ul.second-1] - sumLum[ul.first-1][ul.second-1]))/rectArea(ul, lr);
retVal.a = 1.0;
}

// retVal->h = (atan2(sumHueY[lr.first][lr.second], sumHueX[lr.first][lr.second])*180/PI);
// retVal->s = sumSat[lr.first][lr.second]/rectArea(ul, lr);
// retVal->l = sumLum[lr.first][lr.second]/rectArea(ul, lr);
// retVal->a = 1.0;

return retVal;
}

vector<int> stats::buildHist(pair<int,int> ul, pair<int,int> lr){

// vector<int> secondhisto = hist[ul.first-1][ul.second-1];
// vector<int> firsthisto = hist[lr.first][lr.second];
// vector<int> thirdhisto = hist[ul.first-1][lr.second];
// vector<int> fourthhisto = hist[lr.first][ul.second-1];

vector<int> retVec(36);


if ((ul.first == 0) && (ul.second == 0)) {
    return hist[lr.first][lr.second];
}
else if (ul.first == 0) {
    for (int k = 0; k < 36; k++) {
        retVec[k] = hist[lr.first][lr.second][k] -  hist[lr.first][ul.second-1][k];
    }
}
else if (ul.second == 0) {
    for (int k = 0; k < 36; k++) {   
        retVec[k] =  hist[lr.first][lr.second][k] - hist[ul.first-1][lr.second][k];
    }
}
else {
for (int k = 0; k < 36; k++) {
    retVec[k] = hist[lr.first][lr.second][k] - hist[ul.first-1][lr.second][k] -  hist[lr.first][ul.second-1][k] + hist[ul.first-1][ul.second-1][k];
}
}

return retVec;
}

// takes a distribution and returns entropy
// partially implemented so as to avoid rounding issues.
double stats::entropy(vector<int> & distn,int area){
    

    double entropy = 0.;
    for (int i = 0; i < 36; i++) {
        if (distn[i] > 0 ) {
            entropy += ((double) distn[i]/(double) area) 
                                    * log2((double) distn[i]/(double) area);
        } 
    }
    return  -1 * entropy;

}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){
int area = (int) rectArea(ul, lr);
vector<int> histo = buildHist(ul,lr);
return entropy(histo, area);

}
