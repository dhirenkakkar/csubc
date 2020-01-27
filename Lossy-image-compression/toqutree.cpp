
/**
 *
 * toqutree (pa3)
 * significant modification of a quadtree .
 * toqutree.cpp
 * This file will be used for grading.
 *
 */

#include <assert.h> 

#include "toqutree.h"

toqutree::Node::Node(pair<int,int> ctr, int dim, HSLAPixel a)
	:center(ctr),dimension(dim),avg(a),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
	{}

toqutree::~toqutree(){
	clear(root);
}

toqutree::toqutree(const toqutree & other) {
	root = copy(other.root);
}


toqutree & toqutree::operator=(const toqutree & rhs){
	if (this != &rhs) {
		clear(root);
		root = copy(rhs.root);
	}
	return *this;
}

toqutree::toqutree(PNG & imIn, int k){ 

/* This constructor grabs the 2^k x 2^k sub-image centered */
/* in imIn and uses it to build a quadtree. It may assume  */
/* that imIn is large enough to contain an image of that size. */

// create root node here
	PNG*im = &imIn;

	if (k == 1) {
		pair<int, int> splittingPoint(1, 1);
		pair<int, int> ul(0, 0);
		pair<int, int> lr(1, 1);
		stats* statsTwoByTwo = new stats(*im);
		root = new Node(splittingPoint, k, statsTwoByTwo->getAvg(ul, lr));
		PNG* TileImage = new PNG(pow(2,k+1), pow(2,k+1));

		for (int i = 0; i < im->width(); i++) {
			for (int j = 0; j < im->height(); j++) {
				*TileImage->getPixel(i,j) = *im->getPixel(i,j);						    //tile ul
				*TileImage->getPixel(i+pow(2,k),j+pow(2,k)) = *im->getPixel(i,j);		//tile lr
				*TileImage->getPixel(i,j+pow(2,k)) = *im->getPixel(i,j);				//tile ur
				*TileImage->getPixel(i+pow(2,k),j) = *im->getPixel(i,j);				//tile ll
			}
		}
		
		pair<int, int> seUL = splittingPoint;
		pair<int, int> seLR = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second + pow(2,k-1));

		pair<int, int> swUL = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second);
		pair<int, int> swLR = pair<int,int> (splittingPoint.first + pow(2,k), splittingPoint.second + pow(2,k-1));

		pair<int, int> neUL = pair<int,int> (splittingPoint.first, splittingPoint.second + pow(2,k-1));
	 	pair<int, int> neLR = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second + pow(2,k));

		pair<int, int> nwUL = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second + pow(2,k-1));
		pair<int, int> nwLR = pair<int,int> (splittingPoint.first + pow(2,k), splittingPoint.second + pow(2,k));

	    PNG* seChild = makePNG(TileImage, seUL, seLR, k-1);
	    PNG* swChild = makePNG(TileImage, swUL, swLR, k-1);
	    PNG* nwChild = makePNG(TileImage, nwUL, nwLR, k-1);
	    PNG* neChild = makePNG(TileImage, neUL, neLR, k-1);

		root->NW = buildTree(nwChild, k-1);
		root->NE = buildTree(neChild, k-1);
		root->SW = buildTree(swChild, k-1);
		root->SE = buildTree(seChild, k-1);

		delete(nwChild);
		delete(swChild);
		delete(seChild);
		delete(neChild);
		delete(TileImage);
		delete(statsTwoByTwo);
		nwChild = NULL;
		neChild = NULL;
		seChild = NULL;
		swChild = NULL;
		TileImage = NULL;
		statsTwoByTwo = NULL;
		return;
	}


	if (k == 0) {
		pair<int, int> ctr(0,0);
		HSLAPixel average = *im->getPixel(0,0);
		root = new Node(ctr, k, average);
		root->NW = NULL;
		root->NE = NULL;
		root->SE = NULL;
		root->SW = NULL;
		return;
	} 


    PNG* TileImage = new PNG(pow(2,k+1), pow(2,k+1));

	long minimalEntropy = 0xFFFFFFFF;
	pair<int, int> splittingPoint(0, 0);

	for (int i = 0; i < im->width(); i++) {
		for (int j = 0; j < im->height(); j++) {
			*TileImage->getPixel(i,j) = *im->getPixel(i,j);						    //tile ul
			*TileImage->getPixel(i+pow(2,k),j+pow(2,k)) = *im->getPixel(i,j);		//tile lr
			*TileImage->getPixel(i,j+pow(2,k)) = *im->getPixel(i,j);				//tile ur
			*TileImage->getPixel(i+pow(2,k),j) = *im->getPixel(i,j);				//tile ll
		}
	}

	stats* statsTiledImg = new stats(*TileImage);

	//finding optimal splitting point
	for(int i = k; i < k+pow(2,k-1);i++) {					//try origin if it doesn't work
		for (int j = k; j < k+pow(2,k-1); j++) {

	  		pair<int, int> se(i, j);
	  		pair<int, int> sw(i+pow(2,k-1), j);
	  		pair<int, int> ne(i, j+pow(2,k-1));
	  		pair<int, int> nw(i+pow(2,k-1), j+pow(2,k-1));

	  		pair<int, int> seEND(i+pow(2,k-1), j+pow(2,k-1));
	  		pair<int, int> swEND(i+pow(2,k), j+pow(2,k-1));
	  		pair<int, int> neEND(i+pow(2,k-1), j+pow(2,k));
	  		pair<int, int> nwEND(i+pow(2,k), j+pow(2,k));

	  		long seEntropy = statsTiledImg->entropy(se, seEND);
	  		long swEntropy = statsTiledImg->entropy(sw, swEND);
	  		long neEntropy = statsTiledImg->entropy(ne, neEND);
	  		long nwEntropy = statsTiledImg->entropy(nw, nwEND);

	  		long totalEntropy = seEntropy + swEntropy + neEntropy + nwEntropy;

	  		if (totalEntropy <= minimalEntropy) {
	  			minimalEntropy = totalEntropy;
	  	    	splittingPoint = se;
	  		}
	  	}
	}

	//creation of root
	pair<int,int> ul(0,0);
	pair<int,int> lr(pow(2,k),pow(2,k));

	root = new Node(splittingPoint, k, statsTiledImg->getAvg(ul,lr));
						

	//creation of children
	pair<int, int> seUL = splittingPoint;
	pair<int, int> seLR = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second + pow(2,k-1));

	pair<int, int> swUL = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second);
	pair<int, int> swLR = pair<int,int> (splittingPoint.first + pow(2,k), splittingPoint.second + pow(2,k-1));

	pair<int, int> neUL = pair<int,int> (splittingPoint.first, splittingPoint.second + pow(2,k-1));
	pair<int, int> neLR = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second + pow(2,k));

	pair<int, int> nwUL = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second + pow(2,k-1));
	pair<int, int> nwLR = pair<int,int> (splittingPoint.first + pow(2,k), splittingPoint.second + pow(2,k));

	//helper call
	PNG* seChild = makePNG(TileImage, seUL, seLR, k-1);
	PNG* swChild = makePNG(TileImage, swUL, swLR, k-1);
	PNG* nwChild = makePNG(TileImage, nwUL, nwLR, k-1);
	PNG* neChild = makePNG(TileImage, neUL, neLR, k-1);

	//recursive children call;
	root->NW = buildTree(nwChild, k-1);
	root->NE = buildTree(neChild, k-1);
	root->SW = buildTree(swChild, k-1);
	root->SE = buildTree(seChild, k-1);

	delete(nwChild);
	delete(swChild);
	delete(seChild);
	delete(neChild);
	delete(TileImage);
	delete(statsTiledImg);
	nwChild = NULL;
	neChild = NULL;
	seChild = NULL;
	swChild = NULL;
	TileImage = NULL;
	statsTiledImg = NULL;


}

int toqutree::size() {
/* your code here */
	return size(root);
}


toqutree::Node * toqutree::buildTree(PNG * &im, int k) {

	//two by two base case
	if (k == 1) {
		pair<int, int> splittingPoint(1, 1);
		pair<int, int> ul(0, 0);
		pair<int, int> lr(1, 1);
		stats* statsTwoByTwo = new stats(*im);
		Node * twoByTwo = new Node(splittingPoint, k, statsTwoByTwo->getAvg(ul, lr));
		PNG* TileImage = new PNG(pow(2,k+1), pow(2,k+1));

		for (int i = 0; i < im->width(); i++) {
			for (int j = 0; j < im->height(); j++) {
				*TileImage->getPixel(i,j) = *im->getPixel(i,j);						    //tile ul
				*TileImage->getPixel(i+pow(2,k),j+pow(2,k)) = *im->getPixel(i,j);		//tile lr
				*TileImage->getPixel(i,j+pow(2,k)) = *im->getPixel(i,j);				//tile ur
				*TileImage->getPixel(i+pow(2,k),j) = *im->getPixel(i,j);				//tile ll
			}
		}
		
		pair<int, int> seUL = splittingPoint;
		pair<int, int> seLR = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second + pow(2,k-1));

		pair<int, int> swUL = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second);
		pair<int, int> swLR = pair<int,int> (splittingPoint.first + pow(2,k), splittingPoint.second + pow(2,k-1));

		pair<int, int> neUL = pair<int,int> (splittingPoint.first, splittingPoint.second + pow(2,k-1));
	 	pair<int, int> neLR = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second + pow(2,k));

		pair<int, int> nwUL = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second + pow(2,k-1));
		pair<int, int> nwLR = pair<int,int> (splittingPoint.first + pow(2,k), splittingPoint.second + pow(2,k));

	    PNG* seChild = makePNG(TileImage, seUL, seLR, k-1);
	    PNG* swChild = makePNG(TileImage, swUL, swLR, k-1);
	    PNG* nwChild = makePNG(TileImage, nwUL, nwLR, k-1);
	    PNG* neChild = makePNG(TileImage, neUL, neLR, k-1);

		twoByTwo->NW = buildTree(nwChild, k-1);
		twoByTwo->NE = buildTree(neChild, k-1);
		twoByTwo->SW = buildTree(swChild, k-1);
		twoByTwo->SE = buildTree(seChild, k-1);

		delete(nwChild);
		delete(swChild);
		delete(seChild);
		delete(neChild);
		delete(TileImage);
		delete(statsTwoByTwo);
		nwChild = NULL;
		neChild = NULL;
		seChild = NULL;
		swChild = NULL;
		TileImage = NULL;
		statsTwoByTwo = NULL;

		return twoByTwo;
	}

	//Base case			//ask someone about base case
	if (k == 0) {
		pair<int, int> ctr(0,0);
		HSLAPixel average = *im->getPixel(0,0);
		Node * leaf = new Node(ctr, k, average);
		leaf->NW = NULL;
		leaf->NE = NULL;
		leaf->SE = NULL;
		leaf->SW = NULL;
		return leaf;
	} 

	PNG* TileImage = new PNG(pow(2,k+1), pow(2,k+1));

	long minimalEntropy = 0xFFFFFFFF;
	pair<int, int> splittingPoint(0, 0);

	for (int i = 0; i < im->width(); i++) {
		for (int j = 0; j < im->height(); j++) {
			*TileImage->getPixel(i,j) = *im->getPixel(i,j);						    //tile ul
			*TileImage->getPixel(i+pow(2,k),j+pow(2,k)) = *im->getPixel(i,j);		//tile lr
			*TileImage->getPixel(i,j+pow(2,k)) = *im->getPixel(i,j);				//tile ur
			*TileImage->getPixel(i+pow(2,k),j) = *im->getPixel(i,j);				//tile ll
		}
	}

	stats* statsTiledImg = new stats(*TileImage);
	
	//finding optimal splitting point
	for(int i = k; i < k+pow(2,k-1);i++) {					//try origin if it doesn't work
		for (int j = k; j < k+pow(2,k-1); j++) {

	  		pair<int, int> se(i, j);
	  		pair<int, int> sw(i+pow(2,k-1), j);
	  		pair<int, int> ne(i, j+pow(2,k-1));
	  		pair<int, int> nw(i+pow(2,k-1), j+pow(2,k-1));

	  		pair<int, int> seEND(i+pow(2,k-1), j+pow(2,k-1));
	  		pair<int, int> swEND(i+pow(2,k), j+pow(2,k-1));
	  		pair<int, int> neEND(i+pow(2,k-1), j+pow(2,k));
	  		pair<int, int> nwEND(i+pow(2,k), j+pow(2,k));

	  		long seEntropy = statsTiledImg->entropy(se, seEND);
	  		long swEntropy = statsTiledImg->entropy(sw, swEND);
	  		long neEntropy = statsTiledImg->entropy(ne, neEND);
	  		long nwEntropy = statsTiledImg->entropy(nw, nwEND);

	  		long totalEntropy = seEntropy + swEntropy + neEntropy + nwEntropy;

	  		if (totalEntropy <= minimalEntropy) {
	  			minimalEntropy = totalEntropy;
	  			splittingPoint = se;
	  		}
	  	}
	}


	//creation of root
	pair<int,int> ul(0,0);
	pair<int,int> lr(pow(2,k),pow(2,k));

	Node * curr = new Node(splittingPoint, k, statsTiledImg->getAvg(ul,lr));
						

	//creation of children
	pair<int, int> seUL = splittingPoint;
	pair<int, int> seLR = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second + pow(2,k-1));

	pair<int, int> swUL = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second);
	pair<int, int> swLR = pair<int,int> (splittingPoint.first + pow(2,k), splittingPoint.second + pow(2,k-1));

	pair<int, int> neUL = pair<int,int> (splittingPoint.first, splittingPoint.second + pow(2,k-1));
	 pair<int, int> neLR = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second + pow(2,k));

	pair<int, int> nwUL = pair<int,int> (splittingPoint.first + pow(2,k-1), splittingPoint.second + pow(2,k-1));
	pair<int, int> nwLR = pair<int,int> (splittingPoint.first + pow(2,k), splittingPoint.second + pow(2,k));

	//helper call
	PNG* seChild = makePNG(TileImage, seUL, seLR, k-1);
	PNG* swChild = makePNG(TileImage, swUL, swLR, k-1);
	PNG* nwChild = makePNG(TileImage, nwUL, nwLR, k-1);
	PNG* neChild = makePNG(TileImage, neUL, neLR, k-1);

	//recursive children call;
	curr->NW = buildTree(nwChild, k-1);
	curr->NE = buildTree(neChild, k-1);
	curr->SW = buildTree(swChild, k-1);
	curr->SE = buildTree(seChild, k-1);

	delete(nwChild);
	delete(swChild);
	delete(seChild);
	delete(neChild);
	delete(TileImage);
	delete(statsTiledImg);
	nwChild = NULL;
	neChild = NULL;
	seChild = NULL;
	swChild = NULL;
	statsTiledImg = NULL;
	TileImage = NULL;


	//cout << size() << endl;

	return curr;
}


PNG* toqutree::makePNG(PNG * &im, pair<int,int>ul, pair<int,int>lr, int dim) {

	PNG *img = new PNG(pow(2,dim), pow(2,dim));

	for(int i = ul.first; i < lr.first;i++) {	
		for (int j = ul.second; j < lr.second; j++) {
			*img->getPixel(i-ul.first,j-ul.second) = *im->getPixel(i,j);
	  	}
	}

	return img;
}


PNG toqutree::render(){

// My algorithm for this problem included a helper function
// that was analogous to Find in a BST, but it navigated the 
// quadtree, instead.

	PNG * retVal = render(root);
	PNG localPNG = PNG(pow(2, root->dimension),pow(2, root->dimension));
	localPNG = *retVal;
	delete(retVal);
	retVal = NULL;
	return localPNG;

}

PNG* toqutree::render(Node * &curr) {
	if (curr == NULL) {
		return NULL;
	}

	if ((curr->NW==NULL) && (curr->NE==NULL) && (curr->SE==NULL) && (curr->SW==NULL)) {
		PNG* retVal = new PNG(pow(2, curr->dimension), pow(2, curr->dimension));
		for (int i = 0; i < pow(2, curr->dimension); i++) {
			for (int j = 0; j < pow(2, curr->dimension); j++) {
				*retVal->getPixel(i, j) = curr->avg;
			}
		}
		return retVal;
	}
	
	PNG* nwChild = render(curr->NW);
	PNG* neChild = render(curr->NE);
	PNG* seChild = render(curr->SE);
	PNG* swChild = render(curr->SW);

	PNG* combinedImg = new PNG(pow(2, curr->dimension), pow(2, curr->dimension));

	//bounds are under the assumption that all children have same dim image
	//combines the image

	unsigned int imSize = pow(2, curr->dimension);

	for (int i = 0; i < seChild->width(); i++) {
		for (int j = 0; j < seChild->height(); j++) {
			unsigned int x0 = curr->center.first + i < imSize ? curr->center.first + i: curr->center.first + i - imSize;
	    	unsigned int y0 = curr->center.second + j < imSize ? curr->center.second + j : curr->center.second + j - imSize;
	    	unsigned int x1 = x0 < imSize/2 ? x0 + imSize/2 : x0 - imSize/2;
	     	unsigned int y1 = y0 < imSize/2 ? y0 + imSize/2 : y0 - imSize/2;
			// *combinedImg->getPixel(i,j) = *seChild->getPixel(i,j);						    
			// *combinedImg->getPixel(i+pow(2, curr->dimension-1),j) = *swChild->getPixel(i,j);		
			// *combinedImg->getPixel(i,j+pow(2, curr->dimension-1)) = *neChild->getPixel(i,j);				
			// *combinedImg->getPixel(i+pow(2, curr->dimension-1),j+pow(2, curr->dimension-1)) = *nwChild->getPixel(i,j);	

			*combinedImg->getPixel(x0, y0) = *seChild->getPixel(i,j);						    
			*combinedImg->getPixel(x1, y0) = *swChild->getPixel(i,j);		
			*combinedImg->getPixel(x0, y1) = *neChild->getPixel(i,j);				
			*combinedImg->getPixel(x1,y1) = *nwChild->getPixel(i,j);	
		}
	}

	delete(nwChild);
	delete(neChild);
	delete(swChild);
	delete(seChild);
	nwChild = NULL;
	neChild = NULL;
	swChild = NULL;
	seChild = NULL;

	return combinedImg;
}

/* oops, i left the implementation of this one in the file! */
void toqutree::prune(double tol){
	
	 prune(root,tol);
}



bool toqutree::leavesWithinTol(Node * curr, double tol) {

	if (curr== NULL) {
		return true;
	}

	if ((curr->NW == NULL) && (curr->NW == NULL) && (curr->NW == NULL) && (curr->NW == NULL)) {
		return (abs(root->avg.dist(curr->avg)) <= tol);
	}


	return (leavesWithinTol(curr->NW, tol) && leavesWithinTol(curr->NE, tol) && leavesWithinTol(curr->SW, tol) && leavesWithinTol(curr->SE, tol));
	
}

void toqutree::prune(Node * curr, double tol) {

	if ((curr->NW == NULL) && (curr->NW == NULL) && (curr->NW == NULL) && (curr->NW == NULL)) {
		return;
	}

	int size = pow(2, curr->dimension) * pow(2, curr->dimension);

	if (leavesWithinTol(curr, tol)) {
		clear(curr->NW);
		clear(curr->NE);
		clear(curr->SW);
		clear(curr->SE); //clear all its children (subtrees)

		curr->NW = NULL;
		curr->NE = NULL;
		curr->SE = NULL;
		curr->SW = NULL;
		return;
	}

	prune(curr->NW, tol);
	prune(curr->NE, tol);
	prune(curr->SE, tol);
	prune(curr->SW, tol);
}



/* called by destructor and assignment operator*/
void toqutree::clear(Node * & curr){
	if (curr == NULL) {
		return;
	}

	clear(curr->NW);
	clear(curr->NE);
	clear(curr->SW);
	clear(curr->SE);

	delete curr;

	curr = NULL;
}

/* done */
/* called by assignment operator and copy constructor */
toqutree::Node * toqutree::copy(const Node * other) {
root = new Node(other->center, other->dimension, other->avg);
root->NW = copy(root->NW);
root->NE = copy(root->NE);
root->SE = copy(root->SE);
root->SW = copy(root->SW);
return root;
}

toqutree::Node * toqutree::copy(Node* curr) {
	if ((curr->NW==NULL) && (curr->NE==NULL) && (curr->SE==NULL) && (curr->SW==NULL)) {
		Node* leafNode = new Node(curr->center, curr->dimension, curr->avg);
		leafNode->NW = NULL;
		leafNode->NE = NULL;
		leafNode->SE = NULL;
		leafNode->SW = NULL;
		return leafNode;
	}

	Node* copyNode = new Node(curr->center, curr->dimension, curr->avg);
	copyNode->NW = copy(curr->NW);
	copyNode->NE = copy(curr->NE);
	copyNode->SE = copy(curr->SE);
	copyNode->SW = copy(curr->SW);

	return copyNode;

}





int toqutree::size(Node* &curr) {
	if (curr == NULL) {
		return 0;
	}

	return 1+size(curr->NW)+size(curr->SE)+size(curr->NE)+size(curr->SW);
}
