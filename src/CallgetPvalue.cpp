#include "WeibullR.h"	

	using namespace Rcpp;
	
SEXP CallgetPvalue(SEXP arg1, SEXP arg2, SEXP arg3)  {

int F=Rcpp::as<int>(arg1);
double R2=Rcpp::as<double>(arg2);
int model=Rcpp::as<int>(arg3);

double Pval;
double CCC2=Rcpp::as<double>(CallgetCCC2(arg1,arg3));
// The following code was originally preprepared as a separate ANSI C compliant function, getPvalue(int, double, int).
// The thinking was to facilitate wider use of the function in other software.
// It has been modified here as C++ due to apparant bugs in the MinGW compiler when adding such C code
// errors like "cc1plus.exe: out of memory allocating 139178 bytes" would occur randomly
// and only on the R-Forge build system for 64-bit Windows.

//double CCC2=getCCC2(F,model);

double CritFit=(R2-CCC2)/(1-CCC2);
int n=6; // number of points to interpolate with Aitken, also hard coded within
int base=0;
int Aitken_pts[6]={0};
double Aitken_Pvals[6]={0.0};
double X[6]={0.0};
double Y[6]={0.0};
double x=CritFit;  // just to simplify Aitken code lines
int i=0,j=0,k=0;
double M1[44][23]={{0.0}};
double M2[19][4]={{0.0}};
double	M3a[23][4]={{0.0}};
double	M3b[23][4]={{0.0}};

double M1w[44][23]={{ 0,1.384429,1.026544,1.068409,1.117391,1.180771,1.247442,1.314175,
	 1.384862,1.450565,1.515348,1.578585,1.635932,1.691243,1.743157,1.790174,
	 1.839625,1.881486,1.924153,1.96181,2.001378,2.034937,2.068702}, 
	 {0,1.60367,1.198901,1.27142,1.326677,1.391562,1.461175,1.528318,
	 1.599766,1.666546,1.732863,1.795851,1.853426,1.908636,1.961087,2.008062,
	 2.057697,2.100037,2.143217,2.180591,2.220143,2.255003,2.288135}, 
	 {0,1.842106,1.41032,1.509886,1.57027,1.636235,1.706868,1.775079,
	 1.84707,1.914381,1.979111,2.042738,2.09969,2.155584,2.207351,2.254261,
	 2.30365,2.346737,2.3894,2.426565,2.466503,2.500978,2.534129}, 
	 {0.2030128,2.101266,1.672098,1.78685,1.853588,1.918132,1.989901,2.05831,
	 2.129271,2.197737,2.261492,2.323827,2.380784,2.435121,2.487419,2.532597,
	 2.581595,2.624074,2.667025,2.703714,2.743795,2.778258,2.81077}, 
	 {1.0383681,2.382264,1.993893,2.10787,2.183145,2.245108,2.3145,2.384807,
	 2.453079,2.520177,2.582884,2.644023,2.699291,2.753547,2.805188,2.848079,
	 2.89635,2.937797,2.981501,3.017176,3.056588,3.091029,3.122178}, 
	 {1.8819639,2.686418,2.380406,2.478077,2.564233,2.622096,2.689581,2.757767,
	 2.824061,2.890203,2.949753,3.009267,3.063439,3.115366,3.165485,3.207393,
	 3.253823,3.294021,3.337338,3.371287,3.410283,3.443638,3.473944}, 
	 {2.7336244,3.029175,2.837105,2.90719,3.001824,3.056626,3.120915,3.185245,
	 3.249372,3.314282,3.369287,3.426972,3.477865,3.52894,3.575775,3.615931,
	 3.661208,3.70008,3.740536,3.773211,3.81031,3.842143,3.872622}, 
	 {3.5948654,3.453738,3.369162,3.40303,3.505799,3.55708,3.615888,3.67793,
	 3.737886,3.798112,3.848994,3.903606,3.95304,4.000868,4.043966,4.08162,
	 4.123915,4.161469,4.198635,4.229666,4.265465,4.295588,4.324946}, 
	 {4.466434,3.968107,3.983965,3.976396,4.083597,4.135262,4.187457,4.243119,
	 4.297801,4.352848,4.399957,4.450874,4.495982,4.539538,4.578626,4.614779,
	 4.652732,4.687971,4.722249,4.750665,4.784095,4.811663,4.839292}, 
	 {5.3493158,4.57619,4.688457,4.644624,4.749626,4.800487,4.843963,4.893784,
	 4.941225,4.990585,5.032958,5.077732,5.118018,5.157181,5.191818,5.225302,
	 5.257904,5.290492,5.320986,5.346107,5.376589,5.401464,5.425457}, 
	 {6.2449433,5.29466,5.490406,5.424438,5.51591,5.564893,5.600791,5.641954,
	 5.682433,5.721922,5.761964,5.799042,5.833653,5.866064,5.896126,5.925238,
	 5.953001,5.980032,6.008362,6.030973,6.053621,6.0765,6.098724}, 
	 {7.1547144,6.176034,6.406173,6.332509,6.399368,6.443383,6.472829,6.503339,
	 6.535011,6.566784,6.597489,6.629262,6.656839,6.683093,6.706247,6.730999,
	 6.753633,6.773946,6.794919,6.815125,6.834729,6.85257,6.868408}, 
	 {8.0793294,7.237192,7.447774,7.383123,7.419135,7.45687,7.478141,7.49807,
	 7.518818,7.542447,7.564252,7.58819,7.604975,7.625118,7.642409,7.65873,
	 7.675834,7.688613,7.704709,7.719322,7.733148,7.745933,7.757861}, 
	 {9.0209936,8.493966,8.629756,8.589306,8.600098,8.624309,8.635866,8.647491,
	 8.656535,8.669135,8.681067,8.692897,8.702729,8.713484,8.722445,8.73178,
     8.739386,8.746999,8.756564,8.765006,8.771271,8.77651,8.784822},
    {10.9604955,11.691602,11.493713,11.550236,11.559137,11.526114,11.513352,11.507164,
	11.493907,11.480536,11.469139,11.457898,11.446154,11.435953,11.426137,11.413491,
	11.407801,11.396486,11.388919,11.380442,11.372783,11.364002,11.360595},
	{11.9626811,13.722007,13.227303,13.354929,13.389229,13.329168,13.29841,13.280029,
	13.25659,13.231721,13.206991,13.183739,13.15889,13.137243,13.115079,13.090669,
	13.075447,13.051503,13.037084,13.020071,13.001806,12.985488,12.976155},
	{13.8637801,16.225449,15.20609,15.414979,15.483226,15.421,15.366467,15.335383,
	15.300631,15.267945,15.225246,15.191117,15.153855,15.119956,15.084883,15.045993,
	15.020729,14.9855,14.963183,14.93636,14.905425,14.87923,14.861981},
	{16.6803097,18.967936,17.481726,17.770548,17.883299,17.834497,17.766301,17.718755,
	17.672821,17.629034,17.57644,17.527609,17.481069,17.43185,17.382504,17.330335,
	17.295106,17.246182,17.212132,17.175224,17.13572,17.099619,17.071937},
	{19.5702534,21.857801,20.136802,20.465461,20.627997,20.608998,20.543216,20.482967,
	20.425717,20.370562,20.30999,20.25015,20.191872,20.129844,20.069767,20.005312,
	19.954269,19.894462,19.84963,19.80143,19.748242,19.702793,19.665224},
	{22.5389995,24.909278,23.264075,23.555562,23.766366,23.792459,23.748557,23.690183,
	23.624661,23.56114,23.489117,23.422943,23.351112,23.282379,23.209573,23.132859,
	23.072651,23.000701,22.944704,22.885412,22.81669,22.767061,22.713468},
	{25.5949086,28.14365,26.930372,27.093964,27.360624,27.435603,27.42901,27.39538,
	27.336518,27.262102,27.186691,27.120212,27.035727,26.961204,26.878551,26.789504,
	26.718677,26.639108,26.571293,26.503427,26.425441,26.366805,26.298863},
	{28.7478114,31.586595,31.153116,31.167097,31.479774,31.605687,31.650361,31.653071,
	31.619143,31.550709,31.483145,31.414897,31.324936,31.245448,31.15744,31.062866,
	30.985111,30.898006,30.821642,30.748048,30.660601,30.593368,30.517398},
	{32.0099275,35.273722,35.778118,35.856122,36.197384,36.372804,36.47875,36.527195,
	36.529687,36.495113,36.443072,36.386618,36.303537,36.222081,36.134436,36.037663,
	35.961087,35.863339,35.789613,35.709582,35.619219,35.541963,35.459557},
	{35.39553,39.254584,40.714337,41.175249,41.580604,41.809118,41.980701,42.084497,
	42.14017,42.146724,42.130231,42.101716,42.034548,41.966591,41.890254,41.794764,
	41.72753,41.630226,41.562579,41.47685,41.389662,41.309094,41.22894},
	{37.1407666,41.376728,43.302725,44.045692,44.531385,44.796883,45.00352,45.137368,
	45.222786,45.257176,45.258099,45.251495,45.19797,45.143272,45.076956,44.991504,
	44.928976,44.834687,44.77191,44.688244,44.602406,44.524638,44.446659},
	{38.9246839,43.606277,45.979681,47.025391,47.649998,47.96628,48.213416,48.382012,
	48.499289,48.5592,48.587526,48.599859,48.56675,48.526828,48.471886,48.403288,
	48.348558,48.263221,48.203583,48.128006,48.045513,47.97261,47.900798},
	{40.7498166,45.960804,48.748851,50.107121,50.913231,51.311159,51.6083,51.815555,
	51.964753,52.057684,52.114046,52.148201,52.135372,52.122029,52.081094,52.026408,
	51.980784,51.90942,51.859679,51.791335,51.718821,51.651567,51.584591},
	{42.6199816,48.47305,51.621025,53.291858,54.297431,54.821959,55.181113,55.431357,
	55.624382,55.746111,55.832157,55.895081,55.906879,55.913441,55.891405,55.856184,
	55.828547,55.768094,55.734671,55.671757,55.614039,55.55327,55.499787},
	{44.540546,51.199222,54.604013,56.579772,57.78539,58.466453,58.913687,59.219739,
	59.455696,59.61154,59.733751,59.826544,59.862096,59.896309,59.897982,59.880245,
	59.868914,59.82815,59.808918,59.758936,59.71683,59.668824,59.628882},
	{46.5164908,54.326875,57.700814,59.965099,61.364838,62.210337,62.770094,63.149295,
	63.436831,63.637166,63.797725,63.92253,63.986985,64.045737,64.074157,64.077596,
	64.086784,64.065259,64.065853,64.031871,64.004114,63.966049,63.942294},
	{48.5546333,57.688839,60.917193,63.443658,65.021669,66.02105,66.711452,67.182383,
	67.534031,67.783595,67.987043,68.14871,68.244531,68.32933,68.385736,68.411383,
	68.443738,68.445217,68.464561,68.451539,68.438262,68.416753,68.406299},
	{50.6614743,61.03576,64.246903,67.003461,68.752175,69.88775,70.696652,71.264936,
	71.688371,71.999817,72.251798,72.447727,72.578753,72.69765,72.77806,72.833904,
	72.889617,72.912518,72.955082,72.960103,72.961835,72.967234,72.967069},
	{52.8461756,64.366518,67.696956,70.640266,72.538002,73.784539,74.689199,75.342584,
	75.83769,76.213683,76.514633,76.750668,76.922522,77.072222,77.184578,77.267761,
	77.348081,77.393623,77.458217,77.485115,77.503905,77.529706,77.547218},
	{55.1208496,67.681762,71.273514,74.339603,76.348367,77.678975,78.652415,79.369551,
	79.921261,80.351763,80.695244,80.968805,81.182466,81.362294,81.499234,81.610963,
	81.717746,81.789747,81.872647,81.91648,81.960316,82.002051,82.031515},
	{57.4980342,70.979733,74.972414,78.070841,80.14703,81.525629,82.537659,83.287686,
	83.872269,84.336078,84.708567,85.010134,85.248924,85.452344,85.616415,85.75364,
	85.875769,85.970527,86.066741,86.136852,86.191031,86.252976,86.294827},
	{59.9972463,74.261444,78.76374,81.795396,83.874866,85.261149,86.272484,87.024276,
	87.61507,88.07942,88.460965,88.771113,89.01908,89.233781,89.411081,89.558489,
	89.692068,89.801688,89.907059,89.987526,90.058749,90.12802,90.184418},
	{62.6421864,77.525735,82.519279,85.453036,87.461751,88.798992,89.770229,90.484809,
	91.049223,91.487265,91.84985,92.144806,92.383039,92.589566,92.759358,92.90732,
	93.039945,93.147943,93.252888,93.337502,93.41107,93.48203,93.542403},
	{65.4670948,80.772741,86.121847,88.94483,90.804808,92.032726,92.912379,93.55263,
	94.052045,94.43949,94.753415,95.013155,95.218351,95.397528,95.547293,95.677654,
	95.790601,95.887086,95.979838,96.055099,96.12048,96.180111,96.23853},
	{68.519013,84.001686,89.454866,92.141497,93.784901,94.837478,95.576048,96.101383,
	96.49805,96.807324,97.053872,97.253943,97.411357,97.548165,97.661114,97.75835,
	97.842475,97.914895,97.982519,98.038298,98.088277,98.134601,98.175519},
	{71.8724171,87.213796,92.45566,94.914297,96.260271,97.08257,97.633264,98.01099,
	98.286021,98.496866,98.658046,98.786496,98.886914,98.972147,99.04201,99.098553,
	99.149309,99.191617,99.231389,99.264525,99.292665,99.319431,99.342773},
	{75.6515134,90.406918,95.091259,97.129267,98.12359,98.673179,99.011639,99.230982,
	99.38117,99.489385,99.568964,99.629853,99.675964,99.713393,99.74352,99.766856,
	99.786971,99.80413,99.819089,99.831134,99.842184,99.851106,99.860013},
	{80.0948796,93.581961,97.310997,98.713377,99.307465,99.58927,99.737144,99.82108,
	99.873417,99.906496,99.929142,99.944498,99.956256,99.96432,99.970415,99.97499,
	99.978291,99.981547,99.983835,99.985713,99.987161,99.988765,99.989887},
	{85.8163959,96.737848,99.025261,99.667021,99.872136,99.945695,99.974903,99.987364,
	99.993172,99.996249,99.997756,99.998686,99.999127,99.999427,99.999637,99.999715,
	99.999806,99.999873,99.999889,99.999932,99.999929,99.999947,99.999969},
	{97.224078,99.875073,99.992675,99.99954,99.99995,99.999998,99.999998,100.0,
	100.0,100.0,100.0,100.0,100.0,100.0,100.0,100.0,
	100.0,100.0,100.0,100.0,100.0,100.0,100.0}};

double M1l[44][23]={{ 0.0,0.4525884,1.085988,1.181093,1.242618,1.282548,1.307005,1.322744,
    1.330327,1.337235,1.339658,1.340058,1.339372,1.337299,1.335364,1.331058,
    1.328214,1.325348,1.321593,1.316756,1.31162,1.306158,1.304205},
    {0.0,0.6922538,1.30814,1.385639,1.450519,1.488478,1.511939,1.528083,
    1.534241,1.540762,1.542012,1.541649,1.54096,1.538321,1.53494,1.530687,
    1.527442,1.523826,1.519153,1.513484,1.508619,1.502085,1.499546},
    {0.0,0.9861461,1.561021,1.62228,1.688842,1.7258,1.749324,1.763725,
    1.768617,1.774271,1.775815,1.775001,1.773201,1.769255,1.765561,1.76133,
    1.757516,1.752739,1.747564,1.742178,1.736133,1.729714,1.727247},
    {0.0,1.3356956,1.849013,1.898185,1.965502,2.000505,2.022168,2.036188,
    2.040457,2.044505,2.043697,2.043909,2.040578,2.036158,2.030578,2.027731,
    2.022886,2.018231,2.011514,2.005333,1.999909,1.992582,1.989199},
    {0.0,1.7430406,2.175415,2.217772,2.284823,2.317151,2.337302,2.349601,
    2.352651,2.356191,2.355058,2.353663,2.350076,2.344912,2.338437,2.335179,
    2.329698,2.324233,2.317354,2.309676,2.303859,2.296528,2.292483},
    {0.0,2.210642,2.544482,2.589923,2.65233,2.683298,2.700809,2.710976,
    2.712313,2.715716,2.713893,2.711565,2.706727,2.701147,2.694652,2.690301,
    2.684675,2.678365,2.671494,2.663169,2.656686,2.648673,2.643444},
    {0.0,2.740511,2.966563,3.021706,3.075808,3.10588,3.11961,3.12885,
    3.129145,3.130291,3.127962,3.124312,3.119519,3.113706,3.105999,3.1028,
    3.095051,3.087761,3.08085,3.07253,3.065249,3.05757,3.051949},
    {0.0,3.335648,3.452532,3.521369,3.564357,3.594043,3.603877,3.610939,
    3.610749,3.610602,3.605786,3.601795,3.596786,3.590703,3.582162,3.577965,
    3.570466,3.562975,3.555361,3.54745,3.540011,3.532626,3.526026},
    {0.0,3.9997957,4.010999,4.098609,4.127964,4.157073,4.163038,4.168423,
    4.167162,4.166901,4.16118,4.155996,4.150967,4.144984,4.136359,4.128932,
    4.122406,4.115304,4.107751,4.098523,4.091639,4.084351,4.079447},
    {0.0,4.736299,4.65883,4.764439,4.778482,4.806966,4.810771,4.813569,
    4.812255,4.812054,4.804489,4.799671,4.791916,4.78684,4.778433,4.771334,
    4.764765,4.756662,4.750452,4.74101,4.734561,4.725931,4.721214},
    {0.5445946,5.548964,5.40853,5.534203,5.534445,5.559704,5.559697,5.561954,
    5.559898,5.557537,5.551268,5.544627,5.536882,5.532497,5.52386,5.517196,
    5.511404,5.504988,5.500005,5.488976,5.483841,5.475092,5.469547},
    {2.8417383,6.4439305,6.289323,6.417905,6.411185,6.430689,6.429866,6.429105,
    6.428256,6.424128,6.418132,6.410786,6.406558,6.401392,6.393306,6.386206,
    6.380272,6.375561,6.371337,6.362096,6.356372,6.350204,6.343188},
    {5.1726227,7.4632703,7.327904,7.437477,7.427394,7.439279,7.439075,7.437928,
    7.435057,7.432762,7.425059,7.420011,7.41738,7.414269,7.405758,7.401767,
    7.394649,7.392513,7.389018,7.381418,7.376782,7.372957,7.3673},
    {7.5415065,8.6349637,8.550374,8.613471,8.60543,8.609603,8.610301,8.609434,
    8.606681,8.604656,8.600601,8.598,8.594295,8.593121,8.587777,8.585864,
    8.581363,8.579921,8.578458,8.57355,8.570603,8.568786,8.564901},
    {12.4066806,11.4866874,11.600463,11.540512,11.550195,11.552986,11.552126,11.555595,
    11.560037,11.566104,11.568454,11.571719,11.578185,11.584329,11.585187,11.589946,
    11.593804,11.598421,11.603601,11.606032,11.610049,11.614484,11.616671},
    {14.9108796,13.200973,13.460033,13.359079,13.376672,13.387907,13.395472,13.405792,
    13.414844,13.426678,13.433247,13.444651,13.456088,13.46917,13.479243,13.488761,
    13.501109,13.507637,13.516225,13.527238,13.533782,13.544969,13.554348},
    {17.4693007,15.1346496,15.575103,15.468062,15.488676,15.522834,15.540161,15.559053,
    15.578331,15.599754,15.617205,15.635831,15.659547,15.675428,15.698442,15.714762,
    15.735302,15.749902,15.766206,15.78268,15.795996,15.815012,15.83052},
    {20.0880851,17.3167013,17.979629,17.913633,17.932691,18.000984,18.03379,18.068488,
    18.102578,18.142747,18.168417,18.2026,18.239063,18.264934,18.295258,18.328089,
    18.353517,18.380041,18.40729,18.430413,18.455231,18.481436,18.50679},
    {22.7735745,19.7863204,20.713438,20.740375,20.77024,20.871454,20.932684,20.994491,
    21.044881,21.107562,21.147372,21.201271,21.254452,21.297569,21.34133,21.388561,
    21.426472,21.468813,21.508312,21.542301,21.577959,21.613592,21.655201},
    {25.532509,22.5964996,23.82664,23.990075,24.053795,24.195226,24.303682,24.392033,
    24.471947,24.567605,24.627745,24.710122,24.77556,24.847624,24.904035,24.974316,
    25.028181,25.091921,25.14194,25.190088,25.243688,25.29074,25.350985},
    {28.3753578,25.8315281,27.357075,27.691637,27.844154,28.041356,28.205066,28.338455,
    28.460581,28.59544,28.684499,28.798331,28.888284,28.98784,29.069604,29.164332,
    29.236417,29.322424,29.391125,29.456102,29.529484,29.59405,29.6754},
    {31.3109976,29.6539702,31.349505,31.89875,32.201261,32.470225,32.701535,32.901832,
    33.077987,33.256242,33.390259,33.544372,33.67213,33.798594,33.913843,34.034663,
    34.137029,34.249583,34.340578,34.428153,34.525448,34.609573,34.712607},
    {34.3548084,34.5238618,35.863231,36.660064,37.167724,37.551107,37.87238,38.151625,
    38.394597,38.637419,38.821489,39.028408,39.192029,39.359043,39.5139,39.672367,
    39.803593,39.95153,40.071249,40.182357,40.307147,40.414705,40.541628},
    {37.5216692,39.7384923,40.9835,42.036709,42.769254,43.325562,43.761418,44.140549,
    44.467356,44.782502,45.030623,45.297846,45.516101,45.727573,45.929089,46.129292,
    46.298488,46.474701,46.629044,46.773967,46.930469,47.064866,47.216966},
    {39.1563736,42.4047424,43.775281,44.970174,45.815228,46.476127,46.985349,47.415392,
    47.796543,48.151217,48.437763,48.738399,48.981482,49.224148,49.448995,49.668896,
    49.860996,50.054828,50.227016,50.394268,50.561904,50.714251,50.875131},
    {40.8300888,45.121328,46.726014,48.064573,49.033534,49.800791,50.392824,50.883471,
    51.318041,51.71613,52.041505,52.374687,52.650944,52.920332,53.17011,53.416157,
    53.627847,53.839355,54.03309,54.214029,54.399843,54.568678,54.741056},
    {42.5461943,47.9014332,49.842255,51.319605,52.415848,53.293617,53.976178,54.531236,
    55.022328,55.466758,55.834209,56.199657,56.513693,56.808445,57.083602,57.352918,
    57.58591,57.817717,58.026353,58.228945,58.425602,58.61125,58.800813},
    {44.3084302,50.7703923,53.119574,54.736295,55.957001,56.947443,57.723356,58.347631,
    58.89893,59.390783,59.797119,60.200269,60.546663,60.873542,61.169294,61.457978,
    61.710613,61.965654,62.191678,62.411014,62.622457,62.818868,63.022643},
    {46.1213767,53.8152562,56.568353,58.308706,59.648067,60.747224,61.613095,62.312084,
    62.91766,63.461552,63.905558,64.347421,64.726,65.077403,65.396154,65.702297,
    65.980755,66.247822,66.493127,66.722983,66.953079,67.156617,67.370405},
    {47.9912564,56.9434075,60.119106,62.008638,63.469449,64.66927,65.618846,66.393402,
    67.052222,67.64758,68.128831,68.599744,69.004329,69.379051,69.717957,70.045045,
    70.337442,70.61733,70.881069,71.119375,71.355915,71.571992,71.795915},
    {49.9226915,60.063379,63.67304,65.808631,67.39768,68.683304,69.708995,70.54794,
    71.259497,71.893661,72.410909,72.908613,73.332089,73.727372,74.083769,74.42261,
    74.726273,75.01774,75.28936,75.533612,75.777056,75.999412,76.223227},
    {51.9245899,63.1753947,67.230228,69.675907,71.392011,72.75239,73.842228,74.73011,
    75.478943,76.144794,76.688385,77.202988,77.641788,78.046069,78.412736,78.756268,
    79.067576,79.361382,79.636969,79.880774,80.126757,80.346168,80.565221},
    {54.00506,66.2793636,70.784564,73.547694,75.392334,76.815724,77.942552,78.87203,
    79.642707,80.32086,80.881658,81.401466,81.840815,82.249036,82.615821,82.956267,
    83.263499,83.55118,83.818913,84.056719,84.295034,84.511625,84.721998},
    {56.1772535,69.3738048,74.337408,77.351865,79.327395,80.80604,81.950466,82.884634,
    83.657334,84.331385,84.888891,85.401454,85.829571,86.224715,86.578795,86.903363,
    87.200135,87.467946,87.720103,87.945044,88.169136,88.370623,88.565265},
    {58.4543523,72.4606989,77.891458,81.02574,83.11371,84.620619,85.760481,86.678509,
    87.426236,88.070101,88.605319,89.083901,89.49028,89.856276,90.183967,90.479686,
    90.753159,90.993174,91.220263,91.419932,91.620627,91.797018,91.97237},
    {60.8535421,75.5385075,81.436281,84.543814,86.664972,88.164289,89.271812,90.138744,
    90.839271,91.426123,91.911413,92.33874,92.703918,93.023359,93.307433,93.563687,
    93.796632,94.002678,94.19418,94.36321,94.525593,94.674322,94.818061},
    {63.4013553,78.607701,84.81206,87.861444,89.911602,91.338287,92.370342,93.153879,
    93.776915,94.285134,94.696621,95.055107,95.357091,95.616517,95.84698,96.051841,
    96.233741,96.398119,96.540932,96.670984,96.796525,96.908054,97.01308},
    {66.1293369,81.6708628,87.946351,90.932966,92.800477,94.070682,94.970925,95.633472,
    96.147252,96.554582,96.876256,97.150165,97.376686,97.568234,97.735276,97.879297,
    98.007864,98.120992,98.220328,98.308146,98.390795,98.46392,98.532189},
    {69.0865335,84.7249287,90.829743,93.662063,95.260894,96.30634,97.017409,97.52343,
    97.903971,98.192849,98.415604,98.599104,98.746746,98.870801,98.972866,99.059419,
    99.135798,99.201196,99.259365,99.308008,99.352257,99.391627,99.42873},
    {72.346876,87.7717867,93.429105,95.932228,97.225525,97.997092,98.488749,98.824252,
    99.05972,99.232312,99.358632,99.459201,99.536389,99.597384,99.647554,99.688339,
    99.722651,99.751805,99.77656,99.797267,99.815131,99.830967,99.845046},
    {76.0321697,90.8095231,95.717835,97.698846,98.63469,99.123098,99.404699,99.579641,
    99.69268,99.769168,99.821281,99.859665,99.887305,99.908466,99.923931,99.937003,
    99.946945,99.954563,99.961135,99.96633,99.970715,99.97431,99.977404},
    {80.3825842,93.8411296,97.64852,98.964646,99.497384,99.734658,99.849062,99.909856,
    99.942932,99.963401,99.975315,99.982764,99.987819,99.991289,99.993314,99.995215,
    99.996222,99.997108,99.997749,99.998175,99.99859,99.998861,99.999042},
    {86.0056541,96.8633517,99.145753,99.731435,99.906956,99.965299,99.985557,99.99395,
    99.997142,99.998679,99.999349,99.999613,99.999816,99.999897,99.999943,99.999957,
    99.999983,99.999986,99.99999,99.999991,99.999998,99.999998,100},
    {97.2574665,99.8795326,99.993475,99.999583,99.999966,99.999995,100,100,
    100,100,100,100,100,100,100,100,
    100,100,100,100,100,100,100}};

double M2w[19][4]={{-3.99490824,3.14761023,-0.46196258,0.02144138},
	{-3.79764772,3.15723513,-0.462850242,0.02144896},
	{-3.53050975,3.14307129,-0.459857901,0.021251701},
	{-3.19906325,3.11146234,-0.454402872,0.020948723},
	{-2.78490389,3.05468485,-0.445155977,0.020463032},
	{-2.28515471,2.97482751,-0.432708297,0.019841022},
	{-1.66974429,2.85906041,-0.414895453,0.018962842},
	{-0.9183768,2.69997488,-0.390557278,0.017772453},
	{-0.02336855,2.50006899,-0.360569443,0.016342289},
	{1.03485935,2.25355411,-0.323938375,0.014615217},
	{2.30963381,1.93892555,-0.2771708,0.012404675},
	{3.7609414,1.58908782,-0.226930658,0.010145767},
	{5.52420019,1.13851735,-0.161521879,0.007152023},
	{7.57559514,0.61423061,-0.086512344,0.003789184},
	{12.73138365,-0.69318221,0.096715355,-0.004181639},
	{15.9075161,-1.47210135,0.202440633,-0.008552971},
	{19.51922796,-2.32057012,0.314117485,-0.012929112},
	{23.66032997,-3.25986866,0.435083187,-0.017476986},
	{28.29116986,-4.22980034,0.553725964,-0.021470752}};
	//nrow=19,ncol=4,byrow=TRUE)
double M2l[19][4]={{1.425174,0.12380345,-0.063563029,0.004313224},
    {1.626693,0.13398878,-0.068117776,0.004572437},
    {1.874638,0.13526823,-0.07099075,0.004724446},
    {2.147257,0.14197508,-0.074699728,0.004919587},
    {2.45319,0.15234111,-0.078867223,0.005137342},
    {2.809809,0.15853643,-0.081637413,0.005249301},
    {3.220579,0.16361236,-0.08370464,0.005318358},
    {3.702992,0.16068398,-0.083253248,0.005213982},
    {4.254408,0.15799507,-0.081939284,0.005065951},
    {4.893611,0.14899477,-0.077831631,0.004737024},
    {5.647644,0.12634935,-0.069452969,0.004164364},
    {6.491057,0.11425458,-0.060935892,0.003605794},
    {7.471477,0.09457644,-0.048320758,0.002839868},
    {8.610593,0.06400216,-0.029736233,0.001730182},
    {11.498978,-0.04343311,0.030172069,-0.001672323},
    {13.299306,-0.11212065,0.071939346,-0.003954385},
    {15.387482,-0.19177791,0.12388824,-0.006775469},
    {17.766592,-0.25465699,0.182075147,-0.009887795},
    {20.560408,-0.34211659,0.256460905,-0.01395584}};

double	M3aw[23][4]={{27.40821,-0.9145646,-0.3380048,0.052665835},
	{30.62258,-0.131182,-0.6350816,0.080509377},
	{33.63429,1.2548478,-1.0739771,0.119255615},
	{36.14744,3.4528506,-1.6869067,0.170385123},
	{38.65449,6.110296,-2.3751684,0.225508362},
	{39.76128,7.7040282,-2.7629568,0.255468371},
	{41.17075,9.1824112,-3.1084876,0.281407458},
	{42.23762,10.9773308,-3.5063502,0.31023841},
	{43.80106,12.4826923,-3.8143591,0.331132499},
	{45.41731,13.9912929,-4.0981533,0.349032297},
	{46.3834,15.9944717,-4.4760003,0.373472037},
	{47.49421,17.875563,-4.7981412,0.392516774},
	{49.52189,19.0652638,-4.9371739,0.396805157},
	{52.73569,19.349239,-4.8508925,0.383565222},
	{56.37328,19.2151833,-4.6582883,0.362069142},
	{60.61576,18.5272291,-4.3463501,0.332232682},
	{65.69157,17.0775158,-3.8796308,0.291613345},
	{71.71647,14.757505,-3.2507698,0.240182667},
	{79.30467,11.1268653,-2.3763166,0.172208943},
	{86.73675,7.3778919,-1.5420782,0.110115974},
	{93.10055,4.0136607,-0.8350308,0.059292903},
	{97.64726,1.4409589,-0.3036819,0.021661783},
	{99.68448,0.2057434,-0.0450262,0.003296484}};
	//nrow=23,ncol=4,byrow=TRUE)	

double M3bw[23][4]={{35.43403,-6.27383237,0.85302267,-0.03541931},
	{42.19536,-7.78546664,1.05030732,-0.04302600},
	{49.61464,-9.28291411,1.23946843,-0.04982850},
	{57.96604,-10.84959603,1.43615202,-0.05676211},
	{66.9528,-12.2833641,1.61057312,-0.06239488},
	{71.33879,-12.76368218,1.66032551,-0.06322687},
	{75.58859,-13.05220577,1.68100863,-0.06258365},
	{80.07327,-13.34121409,1.70662873,-0.06244154},
	{84.17001,-13.32511164,1.68883638,-0.06029002},
	{88.03621,-13.10015573,1.64622553,-0.05728040},
	{91.40694,-12.5558799,1.56346694,-0.05274953},
	{94.07532,-11.60962135,1.4277952,-0.04606618},
	{95.93046,-10.25145072,1.24054155,-0.03749309},
	{97.08147,-8.59282494,1.02250057,-0.02835229},
	{97.69882,-6.77715271,0.7960353,-0.01985958},
	{97.6218,-4.78411937,0.55340382,-0.01145380},
	{97.34737,-2.92138359,0.33646903,-0.00502357},
	{97.11475,-1.35462101,0.16001134,-0.00077577},
	{97.17514,-0.2352169,0.03380668,0.00163881},
	{97.83955,0.27027513,-0.02433898,0.00201169},
	{98.76271,0.33797657,-0.03961191,0.00192006},
	{99.63244,0.13738232,-0.01848976,0.00087734},
	{99.97484,0.01130873,-0.00171325,0.00008709}};
	//nrow=23,ncol=4,byrow=TRUE)

double	M3al[23][4]={{23.80125,-0.41880248,0.336832532,-0.017861489},
    {27.52682,-0.53287852,0.455469289,-0.025795222},
    {31.35219,-0.30481097,0.51298792,-0.02929984},
    {35.08245,0.43153657,0.480218578,-0.027244594},
    {39.07448,1.47780078,0.403201949,-0.023829328},
    {40.88214,2.34037954,0.294060864,-0.01768722},
    {42.83818,3.24373358,0.178987684,-0.011864915},
    {45.19703,4.00724193,0.098738226,-0.009637697},
    {46.84815,5.44055774,-0.1440618,0.004509473},
    {48.85201,6.75091689,-0.358782214,0.015082796},
    {51.36313,7.83940002,-0.533030825,0.02161378},
    {53.31699,9.47988642,-0.863571473,0.040184718},
    {55.16148,11.31885939,-1.274033368,0.065128771},
    {57.67573,12.77209629,-1.639355635,0.087575062},
    {60.71284,13.89122082,-1.982274738,0.110300667},
    {64.74392,14.27391526,-2.219249071,0.128268715},
    {69.52708,13.97464866,-2.361462842,0.143256891},
    {75.35577,12.60673611,-2.297093556,0.14639634},
    {82.40041,9.84492056,-1.910686838,0.127245658},
    {89.75416,6.16106649,-1.261174358,0.087378356},
    {95.82351,2.65544543,-0.567400485,0.040644311},
    {99.0983,0.59920465,-0.132762224,0.00980029},
    {99.94641,0.03683011,-0.008399735,0.00063579}};


double	M3bl[23][4]={{23.1446,-0.080424611,0.2827701,-0.01534528},
    {26.72433,-0.064224631,0.3655928,-0.02015776},
    {30.50669,0.174149743,0.4248644,-0.02408533},
    {34.82301,0.519660718,0.478977,-0.02846354},
    {39.18559,1.293068275,0.4674056,-0.0301867},
    {41.37521,1.867417888,0.428637,-0.02964526},
    {43.64282,2.535859427,0.3708724,-0.02838361},
    {45.76205,3.433447878,0.2661572,-0.02471315},
    {47.92357,4.449963633,0.1315719,-0.01963137},
    {50.44208,5.434182515,-0.0141573,-0.01392852},
    {53.08036,6.493171057,-0.1935652,-0.006190151},
    {55.83465,7.609243581,-0.4097597,0.004085162},
    {59.10674,8.536997713,-0.6260755,0.01522055},
    {63.08931,9.109884705,-0.8159845,0.02605237},
    {67.99459,9.124973525,-0.942823,0.03477899},
    {73.82076,8.457601014,-0.9765046,0.03973077},
    {80.44009,7.025780787,-0.8858037,0.03874344},
    {87.33845,4.961969798,-0.6705821,0.0309888},
    {93.46297,2.748242287,-0.3925302,0.01894755},
    {97.62401,1.058680199,-0.1584387,0.007942469},
    {99.53742,0.215337803,-0.03339421,0.001723562},
    {99.97018,0.014351462,-0.002288558,0.000120927},
    {99.99975,0.000120758,-0.000019461,0.000001037}};

double CFpoints[45]={-0.701,-0.651,-0.601,-0.551,-0.501,-0.451,-0.401,-0.351,-0.301,
	-0.251,-0.201,-0.151,-0.101,-0.051,0,0.049,0.099,0.149,
	0.199,0.249,0.299,0.349,0.399,0.449,0.499,0.524,0.549,
	0.574,0.599,0.624,0.649,0.674,0.699,0.724,0.749,0.774,
    0.799,0.824,0.849,0.874,0.899,0.924,0.949,0.974,0.999};

if(model==1)  {
    for( i=0; i<44; i++)  {
        for( j=0; j<23; j++)  {
            M1[i][j]=M1w[i][j];
    }}
    for( i=0; i<19; i++)  {
        for( j=0; j<4; j++)  {
            M2[i][j]=M2w[i][j];
    }}
    for( i=0; i<23; i++)  {
        for( j=0; j<4; j++)  {
            M3a[i][j]=M3aw[i][j];
    }}
    for( i=0; i<23; i++)  {
        for( j=0; j<4; j++)  {
            M3b[i][j]=M3bw[i][j];
    }}
}
else   {
    for( i=0; i<44; i++)  {
        for( j=0; j<23; j++)  {
            M1[i][j]=M1l[i][j];
    }}
    for( i=0; i<19; i++)  {
        for( j=0; j<4; j++)  {
            M2[i][j]=M2l[i][j];
    }}
    for( i=0; i<23; i++)  {
        for( j=0; j<4; j++)  {
            M3a[i][j]=M3al[i][j];
    }}
    for( i=0; i<23; i++)  {
        for( j=0; j<4; j++)  {
            M3b[i][j]=M3bl[i][j];
    }}
    }

// get the base position for Aitken points
for(i=0; i<45; i++)
{
	if(CritFit>CFpoints[i])
	{
		base=i;
	}
}

if(base>1 && base<40)
{
	// This is all hard coded for n=6
    for(i=-2, j=0; i<4; i++,j++)
	{
		Aitken_pts[j]=base+i;
	}
}
else
{
	if(base<2) 
	{
        for(i=0; i<n; i++)
		{
			Aitken_pts[i]=i;
		}
	}
	else if(base>39)
		{
			//Aitken_pts={37,38,39,40,41,42};
            for( i=0; i<n; i++)
			{
				Aitken_pts[i]=i+37;
			}
		}
}

if(F<26)
{
	// Pvalues for Aitken are read right off the M1 matrices
    for( j=0; j<6; j++)
	{
		if(Aitken_pts[j]<14)
		{
            Aitken_Pvals[j]=M1[Aitken_pts[j]][F-3];
		}
		else if(Aitken_pts[j]==14)
		{
			Aitken_Pvals[j]=10.0;
		}
		else
		{
            //Bug found testing abremPivotals 5-19-2014
			//Original code adjusted the index into M1 in the wrong direction
            Aitken_Pvals[j]=M1[(Aitken_pts[j]-1)][F-3];
		}

	}
}
else
{
// the condition F>1500 will be trapped early in by the calling function									
// the rest of Pvalues are determined by polynomial functions
    for( j=0; j<6; j++)
	{
		if(Aitken_pts[j]<14)
		{
			Aitken_Pvals[j]=M2[Aitken_pts[j]][0]+log(F)*M2[Aitken_pts[j]][1]+pow(log(F),2)*M2[Aitken_pts[j]][2]+pow(log(F),3)*M2[Aitken_pts[j]][3];
		}
		else if(Aitken_pts[j]==14)
		{
			Aitken_Pvals[j]=10.0;
		}
		else if(Aitken_pts[j]<20)
		{

			Aitken_Pvals[j]=M2[Aitken_pts[j]-1][0]+log(F)*M2[Aitken_pts[j]-1][1]+pow(log(F),2)*M2[Aitken_pts[j]-1][2]+pow(log(F),3)*M2[Aitken_pts[j-1]][3];
		}
		else if(Aitken_pts[j]<43)
		{
			if(F<91) 
			{

				Aitken_Pvals[j]=M3a[Aitken_pts[j]-20][0]+log(F)*M3a[Aitken_pts[j]-20][1]+pow(log(F),2)*M3a[Aitken_pts[j]-20][2]+pow(log(F),3)*M3a[Aitken_pts[j]-20][3];
			}
			else
			{
				Aitken_Pvals[j]=M3b[Aitken_pts[j]-20][0]+log(F)*M3b[Aitken_pts[j]-20][1]+pow(log(F),2)*M3b[Aitken_pts[j]-20][2]+pow(log(F),3)*M3b[Aitken_pts[j]-20][3];
			}
		}
		else
		{
			Aitken_Pvals[j]=100.0;
		}
	}
}
	// Early placement of Aitken interpolation for initial code test

	// Prepare X and Y arrays for Aitken interpolation
    for(i=0; i<n; i++)
	{
		X[i]=CFpoints[Aitken_pts[i]];
		Y[i]=Aitken_Pvals[i]; 
	}

	// This is the Aiken interpolation
    for(i=1; i<n; i++)
	{
		k=0;
		while((k+i)<n)
		{
			Y[k]=(x-X[k])/(X[k+i]-X[k])*Y[k+1]+(x-X[k+i])/(X[k]-X[k+i])*Y[k];
			k=k+1;
		}
	}

//	outStruct.Pval=Y[0];
//    if(outStruct.Pval>100.0) outStruct.Pval=100.0;
//    if(outStruct.Pval<0.0) outStruct.Pval=0.0;
Pval=Y[0];
    if(Pval>100.0) Pval=100.0;
    if(Pval<0.0) Pval=0.0;
//outStruct.CCC2=CCC2;


//return outStruct;




//struct AbPval Zzstruct;
//Zzstruct=getPvalue(F,R2,model);

NumericVector Outvals(2);

//Outvals[0]=Zzstruct.Pval;
//Outvals[1]=Zzstruct.CCC2;
Outvals[0]=Pval;
Outvals[1]=CCC2;

return Outvals;
}
