function U = ocam_undistort_map(OCAM_MODEL, varargin)
%
% U = ocam_undistort_map(OCAM_MODEL[, UNDIST_RES, Name, Value, ...] )
% 
% builds a undistortion map and calculates the perspective camera
% matrix K for a camera modeled with the OCamCalib toolbox 
%
% --- input -----
% 
% OCAM_MODEL  the calibration camera model as obtained from OCamCalib
%             toolbox
% UNDIST_RES  (optional) the resolution of the undistorted image
%             defaults to standard resolution
% Name, Value (optional) additional parametes
%             'OutputView'    'same' undistorted image is the same size
%                                    as  distorted image
%                             'full' undistorted image contains all
%                                    pixels of distorted image
%                             'fov'  specify field of fov explicitly
%
% --- output -----
%
% U         structure with the following fields:
% 
%           U.map the undistortion map to be used with ocam_undistort
%           U.K   the intrinsic camera matrix K for a perspective camera,
%                 same convention as matlab IntrinsicMatrix
%           U.res target resolution
%
% 2016 Bernd Pfrommer
%
    [target_res, mode] = get_res_and_mode(OCAM_MODEL, varargin);
    %
    % precompute inverse polynomial if necessary
    %
    if ~isfield(OCAM_MODEL,'pol') 
        width  = OCAM_MODEL.width;
        height = OCAM_MODEL.height;
        %The OCAM_MODEL does not contain the inverse polynomial pol
        OCAM_MODEL.pol = findinvpoly(OCAM_MODEL.ss,sqrt((width/2)^2+(height/2)^2));
    end
    assert(isfield(OCAM_MODEL, 'pol'));
    %
    % create an intrinsic matrix K that fits the required resolution and
    % mode
    %
    k = make_intrinsic_matrix(OCAM_MODEL, target_res, mode, varargin);
    %
    % set up a rectangular even grid in the undistorted image pixel
    % coordinates. This grid corresponds to destination pixels
    % of the undistorted, perspective image.
    %
    [nx, ny] = meshgrid(1:target_res(1), 1:target_res(2));
    %
    % now shift and invert to go from pixel coordinates to 
    % homogenous world coordinates
    %
    n = [nx(:)'-k(1,3); ny(:)'-k(2,3)];
    x = (k(1:2,1:2)^-1) * n;
    xh = [-x(1,:); -x(2,:); -ones(1, target_res(1) * target_res(2))];
    %
    % Project the world coordinates back to distorted pixel coordinates to
    % get a fast lookup table
    %
    U.map = world2cam_fast(normc(xh), OCAM_MODEL)';
    %
    % transpose intrinsic matrix to follow matlab convention
    %
    U.K = k';
    U.res = target_res;
end

function k = make_intrinsic_matrix(ocam_model, target_res, mode, params)
    if mode == 0
        k = make_k_corners_visible(ocam_model, target_res);
    else
        if  mode == 1
            k = make_k_same(ocam_model, target_res);
        else
            if mode == 2
                k = make_k_fov(ocam_model, target_res, params);
            else
                error('invalid mode!');
            end
        end
    end
end

function k = make_k_fov(ocam_model, target_res, params)
%
% makes a k-matrix for a specified field of view
%
    nparams = length(params);
    fovx = -1;
    fovy = -1;
    for i = 1:nparams
        if strcmpi(params{i},'fovx') & i < nparams
            fovx = params{i+1};
        end
        if strcmpi(params{i},'fovy') & i < nparams
            fovy = params{i+1};
        end
    end
    if fovx < 0 | fovy < 0
        error('fovx or fovy incorrect or not specified!');
    end
    if fovx >= 180 | fovy >= 180
        error('fovx or fovy out of range');
    end
%
% make k such that there is a certain fov in x and y
%
    tana2x = tan(0.5*fovx/180*pi); % incoming angle in x
    tana2y = tan(0.5*fovy/180*pi); % incoming angle in y
    xmin = -tana2x; xmax = tana2x; % smallest/largest X/Z
    ymin = -tana2y; ymax = tana2y;
    umin = 1; umax = target_res(1);
    vmin = 1; vmax = target_res(2);
    k = make_k(xmin, xmax, ymin, ymax, umin, umax, vmin, vmax);
end

function k = make_k_corners_visible(ocam_model, target_res)
%
% make k such that top left and bottom right corners
% of distorted and undistorted image line up
%
    imageCorners = [[1, 1]', [ocam_model.width, ocam_model.height]'];
    xworld = cam2world(imageCorners, ocam_model);
    X = [xworld(1,:)./xworld(3,:); xworld(2,:)./xworld(3,:); ones(1, size(xworld,2))];
    xmin = X(1,1); xmax = X(1,2);
    ymin = X(2,1); ymax = X(2,2);
    umin = 1; umax = target_res(1);
    vmin = 1; vmax = target_res(2);

    k = make_k(xmin, xmax, ymin, ymax, umin, umax, vmin, vmax);
end

%
% Make K such that projected image fills the old one completely
% This is similar to the matlab 'same' mode.
%

function k = make_k_same(ocam_model, target_res)
    w = ocam_model.width;
    h = ocam_model.height;
    imageCorners = [[1, h/2]', [w/2, 1]', [w, h/2]', [w/2, h]'];
    xworld = cam2world(imageCorners, ocam_model);
    X = [xworld(1,:)./xworld(3,:); xworld(2,:)./xworld(3,:); ones(1, size(xworld,2))];
    xmin = X(1,1); xmax = X(1,3);
    ymin = X(2,2); ymax = X(2,4);
    umin = 1; umax = target_res(1);
    vmin = 1; vmax = target_res(2);
    k = make_k(xmin, xmax, ymin, ymax, umin, umax, vmin, vmax);
end

function k = make_k(xmin, xmax, ymin, ymax, umin, umax, vmin, vmax)
    zmax = 1;
    zmin = 1;
    fx = (umax*zmax - umin*zmin) / (xmax-xmin);
    fy = (vmax*zmax - vmin*zmin) / (ymax-ymin);
    cx = (zmin*umin - fx * xmin) / zmin;
    cy = (zmin*vmin - fy * ymin) / zmin;
    k=[fx, 0, cx; 0, fy, cy; 0, 0, 1];
end

function [res, mode] = get_res_and_mode(model, varg)
%
% parses input arguments.
%
    res = [model.width, model.height];
    mode = 0;
    if length(varg) <= 0
        return
    end

    istart = 1;
    if isnumeric(varg{istart})
        res = varg{istart};
        istart = istart + 1;
    end
    for i = istart:length(varg)
        if strcmpi(varg{i}, 'OutputView')
            if (i >= length(varg))
                error('no value provided for OutputView');
            end
            val = varg{i+1};
            if ~ischar(val)
                error('value for OutputView must be string');
            end
            if strcmpi(val, 'full')
                mode = 0;
            else
                if strcmpi(val, 'same')
                    mode = 1;
                else
                    if strcmpi(val, 'fov')
                        mode = 2;
                    else
                        error('invalid value for OutputView');
                    end
                end
            end
        end
    end
end

%
% ----------------------------------------------------------------------
% copied from OCAMCALIB toolbox and hacked to modify the orientation
% of the axis.
%


%CAM2WORLD Project a give pixel point onto the unit sphere
%   M=CAM2WORLD=(m, ocam_model) returns the 3D coordinates of the vector
%   emanating from the single effective viewpoint on the unit sphere
%
%   m=[rows;cols] is a 2xN matrix containing the pixel coordinates of the image
%   points.
%
%   "ocam_model" contains the model of the calibrated camera.
%   
%   M=[X;Y;Z] is a 3xN matrix with the coordinates on the unit sphere:
%   thus, X^2 + Y^2 + Z^2 = 1
%   
%   Last update May 2009
%   Copyright (C) 2006 DAVIDE SCARAMUZZA   
%   Author: Davide Scaramuzza - email: davide.scaramuzza@ieee.org

function M=cam2world(m, ocam_model)

n_points = size(m,2);

ss = ocam_model.ss;
xc = ocam_model.xc;
yc = ocam_model.yc;
c = ocam_model.c;
d = ocam_model.d;
e = ocam_model.e;

A = [1, e;
     d, c];
T = [yc;xc]*ones(1,n_points);
m = A^-1*(m-T);
M = getpoint(ss,m);
M = normc(M);

end

function w=getpoint(ss,m)

% Given an image point it returns the 3D coordinates of its correspondent optical
% ray

w = [-m(1,:) ; -m(2,:) ; polyval(ss(end:-1:1),sqrt(m(1,:).^2+m(2,:).^2)) ];

end

%FINDINVPOLY finds the inverse polynomial specified in the argument.
%   [POL, ERR, N] = FINDINVPOLY(SS, RADIUS, N) finds an approximation of the inverse polynomial specified in OCAM_MODEL.SS.
%   The returned polynomial POL is used in WORLD2CAM_FAST to compute the reprojected point very efficiently.
%   
%   SS is the polynomial which describe the mirrror/lens model.
%   RADIUS is the radius (pixels) of the omnidirectional picture.
%   ERR is the error (pixel) that you commit in using the returned
%   polynomial instead of the inverse SS. N is searched so that
%   that ERR is < 0.01 pixels.
%
%   Copyright (C) 2008 DAVIDE SCARAMUZZA, ETH Zurich
%   Author: Davide Scaramuzza - email: davide.scaramuzza@ieee.org

function [pol, err, N] = findinvpoly(ss, radius)

if nargin < 3
    maxerr = inf;
    N = 1;
    while maxerr > 0.01 %Repeat until the reprojection error is smaller than 0.01 pixels
        N = N + 1;
        [pol, err] = findinvpoly2(ss, radius, N);
        maxerr = max(err);  
    end
else
    [pol, err, N] = findinvpoly2(ss, radius, N)
end

end

function [pol, err, N] = findinvpoly2(ss, radius, N)

theta = [-pi/2:0.01:1.20];
r     = invFUN(ss, theta, radius);
ind   = find(r~=inf);
theta = theta(ind);
r     = r(ind);

pol = polyfit(theta,r,N);
err = abs( r - polyval(pol, theta)); %approximation error in pixels

end

function r=invFUN(ss, theta, radius)

m=tan(theta);

r=[];
poly_coef=ss(end:-1:1);
poly_coef_tmp=poly_coef;
for j=1:length(m)
    poly_coef_tmp(end-1)=poly_coef(end-1)-m(j);
    rhoTmp=roots(poly_coef_tmp);
    res=rhoTmp(find(imag(rhoTmp)==0 & rhoTmp>0 & rhoTmp<radius ));
    if isempty(res) | length(res)>1
        r(j)=inf;
    else
        r(j)=res;
    end
end

end

%WORLD2CAM projects a 3D point on to the image
%   m=WORLD2CAM_FAST(M, ocam_model) projects a 3D point on to the
%   image and returns the pixel coordinates. This function uses an approximation of the inverse
%   polynomial to compute the reprojected point. Therefore it is very fast.
%   
%   M is a 3xN matrix containing the coordinates of the 3D points: M=[X;Y;Z]
%   "ocam_model" contains the model of the calibrated camera.
%   m=[rows;cols] is a 2xN matrix containing the returned rows and columns of the points after being
%   reproject onto the image.
%   
%   Copyright (C) 2008 DAVIDE SCARAMUZZA, ETH Zurich  
%   Author: Davide Scaramuzza - email: davide.scaramuzza@ieee.org

function m = world2cam_fast(M, ocam_model)

ss = ocam_model.ss;
xc = ocam_model.xc;
yc = ocam_model.yc;
c = ocam_model.c;
d = ocam_model.d;
e = ocam_model.e;
pol = ocam_model.pol;

npoints = size(M, 2);
theta = zeros(1,npoints);

NORM = sqrt(M(1,:).^2 + M(2,:).^2);

ind0 = find( NORM == 0); %these are the scene points which are along the z-axis
NORM(ind0) = eps; %this will avoid division by ZERO later

theta = atan( M(3,:)./NORM );

rho = -polyval( pol , theta ); %Distance in pixel of the reprojected points from the image center

x = M(1,:)./NORM.*rho ;
y = M(2,:)./NORM.*rho ;

%Add center coordinates
m(1,:) = x*1 + y*e  + yc;
m(2,:) = x*d + y*c  + xc;

end