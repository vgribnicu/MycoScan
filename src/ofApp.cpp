#include "ofApp.h"
#include "ofxKuFile.h"
#include "ofxKuGeomMeshUtils.h"

vector<ofPoint> p_;
ofTexture image_;


//is previous point
bool prev_ = false;
ofPoint prev_p_;


//--------------------------------------------------------------
void ofApp::setup(){
	string Title = "Mycoscan 1.0";
	ofSetWindowTitle(Title);
	ofSetFrameRate(60);

	cout << "==============================================" << endl;
	cout << Title << endl;
	cout << "Keys (please use English layout, not Russian!):" << endl;
	cout << "    Space - end current line, Backspace - delete last, L - reload, S - save, Esc - exit" << endl;
	cout << "    Enter - export OBJ" << endl;
	cout << "Mouse: left click - create" << endl;
	cout << "==============================================" << endl;

	ofLoadImage(image_, "image.jpg");
	cout << "Loaded image " << image_.getWidth() << " x " << image_.getHeight() << endl;

	load();

}

//--------------------------------------------------------------
void ofApp::exit() {
	save();
}

//--------------------------------------------------------------
void ofApp::load() {
	cout << "Loading" << endl;
	auto file = ofxKuFileReadStrings("myco.ini");
	int n = file.size();
	p_.resize(n);
	for (int i = 0; i < n; i++) {
		auto list = ofSplitString(file[i], " ");
		if (list.size() < 2) {
			ofSystemAlertDialog("Bad line " + ofToString(i + 1) + ": '" + file[i] + "'");
			return;
		}
		p_[i] = ofPoint(ofToFloat(list[0]), ofToFloat(list[1]));
	}

}

//--------------------------------------------------------------
void ofApp::save() {
	cout << "Saving" << endl;

	int n = p_.size();
	vector<string> file(n);
	for (int i = 0; i < n; i++) {
		file[i] = ofToString(p_[i].x) + " " + ofToString(p_[i].y);
	}
	ofxKuFileWriteStrings(file, "myco.ini");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
float get_scale() {
	float W = ofGetWidth();
	float H = ofGetHeight();
	int w = image_.getWidth();
	int h = image_.getHeight();
	if (w == 0 || h == 0) return 1;
	return min(W / w, H / h);
}

//--------------------------------------------------------------
//find nearest point or new point
ofPoint get_point(int mousex, int mousey) {
	float Rad = 5;

	float scl = get_scale();
	ofPoint P(mousex / scl, mousey / scl);
	for (auto& p : p_) {
		if (ofDist(p.x, p.y, P.x, P.y) < Rad) {
			return p;
		}
	}
	return P;

}


//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);
	ofSetColor(255);
	ofPushMatrix();
	float scl = get_scale();
	ofScale(scl, scl);

	int img_color = 200;			//draw image darker
	ofSetColor(img_color);
	image_.draw(0, 0);

	//points
	ofSetColor(255, 0, 0);
	ofSetLineWidth(3);
	int n = p_.size();
	n = n / 2 * 2;	//elimimate odd count
	for (int i = 0; i < n; i += 2) {
		ofLine(p_[i], p_[i + 1]);
	}
	ofSetLineWidth(1);
	ofSetColor(0, 0, 255);
	ofFill();
	for (int i = 0; i < n; i++) {
		ofDrawRectangle(p_[i].x - 3, p_[i].y - 3, 6, 6);
	}


	//line from previous and current mouse position
	ofPoint M = get_point(mouseX, mouseY);

	if (prev_) {
		ofSetLineWidth(3);
		ofSetColor(0, 0, 255);
		ofLine(prev_p_, M);
		ofSetLineWidth(1);
	}

	//mouse position
	float cross = 20 / scl;

	ofFill();
	ofSetColor(0,0,255);
	ofCircle(M.x, M.y, cross / 3);
	ofNoFill();
	ofSetColor(0, 255, 0);
	ofCircle(M.x, M.y, cross / 3);

	ofSetColor(0,255,0);
	ofLine(M.x - cross, M.y, M.x + cross, M.y);
	ofLine(M.x, M.y - cross, M.x, M.y + cross);

	ofPopMatrix();


}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 's') save();
	if (key == 'l') load();
	if (key == OF_KEY_BACKSPACE) {
		int n = p_.size();
		if (n >= 2) {
			p_.resize(n - 2);
			prev_ = false;
		}
	}
	if (key == ' ') prev_ = false;

	if (key == OF_KEY_RETURN) export_mesh();

}

//--------------------------------------------------------------
void ofApp::export_mesh() {	//export mesh as degenerated rectangles to lines for using in Cinema 4D for voxel builder/mesher
	ofMesh mesh;
	auto& V = mesh.getVertices();
	int n = p_.size();
	
	if (n == 0) return;
	//centering
	double X = 0;
	double Y = 0;
	double Z = 0;
	for (auto& p : p_) {
		X += p.x;
		Y += p.y;
		Z += p.z;
	}
	ofPoint center(X / n, Y / n, Z / n);

	//TODO join points
	V.resize(n);
	for (int i = 0; i < n; i++) {
		V[i] = p_[i] - center;
	}
	for (int i = 0; i < n; i += 2) {
		mesh.addTriangle(i, i + 1, i + 1);
		mesh.addTriangle(i, i + 1, i);

	}

	ofxKuSaveObjFile(mesh, "mesh.obj", false, false);
	ofSystemAlertDialog("Mesh exported");
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	ofPoint p = get_point(x, y);
	if (prev_) {
		p_.push_back(prev_p_);
		p_.push_back(p);
	}

	prev_ = true;
	prev_p_ = p;

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
