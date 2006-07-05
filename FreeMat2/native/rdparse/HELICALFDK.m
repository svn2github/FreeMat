% -----------------------------------------------------------------------
% GE proprietary and confidential
% CatSim 2.0
%
% Routine
%   HELICALFDK
%
% Authors
%   Samit Basu and Bruno De Man (GE Global Research)
%
% Aim
%   Performs a 3D helical FDK jr reconstruction from 
%   a CatSim sinogram.  The output image is in HU.
%
% Inputs
%   cfg.recon_fov      - reconstructed FOV (diameter)
%   cfg.recon_size     - the size of the image (in pixels)
%   cfg.recon_xcenter  - center of the image in X
%   cfg.recon_ycenter  - center of the image in Y
%   cfg.recon_filter   - the filter to use.
%
% Outputs
%   cfg.recon_filename - where to write the results file
% -----------------------------------------------------------------------
function HELICALFDK(cfg, detectorFlat)
  try
    eMu = cfg.EffectiveMu;
  catch
    error('Simulation was done without water-based beam hardening.  Add cfg.EffectiveMu = 0.02 (or some value), enable WaterBHCCorrection, and rerun the simulation.');
  end
  % Fill out the detector config
  cfg.x0 = 0;
  cfg.y0 = cfg.sid;
  cfg.detUCount = cfg.col_count;
  cfg.detVCount = cfg.row_count;
  cfg.detUSize = cfg.col_size;
  cfg.detVSize = cfg.row_size;
  cfg.detUCenter = cfg.col_center;
  cfg.detVCenter = cfg.row_center;
  % Generate the detector coordinates
  if (~detectorFlat) 
    cfg.sdd = cfg.sid + cfg.did;
    alphas = (((1:cfg.col_count) - cfg.col_center)*cfg.col_size)/cfg.sdd;
    cfg.xds = cfg.sdd*sin(alphas);
    cfg.yds = (cfg.sid-cfg.sdd*cos(alphas));
  else
    cfg.xds = ((1:cfg.col_count) - cfg.col_center)*cfg.col_size;
    cfg.yds = ones(1,cfg.col_count)*(cfg.sid-cfg.sdd);
  end
  % These are the processing steps...
  % First, we rebin from fan to parallel
  total_views = cfg.n_rotations*cfg.views_per_rotation;
  delbeta = 2*pi/cfg.views_per_rotation;
  numout = fan2par_helical_file(cfg.results_filename,...
                                sprintf('%s_rebin',cfg.recon_filename),...
                                cfg,delbeta,4,4,total_views);
  P = (total_views - numout)/2.0;
  rampfilter_flat_file(sprintf('%s_rebin',cfg.recon_filename),...
                       sprintf('%s_filt',cfg.recon_filename),cfg,numout,delbeta);
  for cv = cfg.recon_centerviews
    printf('Reconstructing at centerview %d...\n',cv);
    numviews_out = view_weight_file(sprintf('%s_filt',cfg.recon_filename),...
                                    sprintf('%s_weight',cfg.recon_filename),...
                                    cfg,numout,cv-P,cfg.recon_weighting_scheme,...
                                    delbeta);
    img = bp_semicone_flat_file(sprintf('%s_weight',cfg.recon_filename),cfg,-delbeta,...
                                cfg.table_speed*cfg.rotation_period,numviews_out,cv);
    fp = fopen(sprintf('%s_slice.%d',cfg.recon_filename,cv),'wb');
    fwrite(fp,float(img));
    fclose(fp);
  end



% Calculate the Lagrange coefficients to interpolate x from signals at xk
% Output is a matrix of the same size as xk
function coeff = get_lagrange_coeff(xk,x)
  coeff = zeros(size(xk));
  M = size(xk,1);
  L = size(xk,2);
  for i=1:M
    xkvec = xk(i,:);
    dnom = repmat(xkvec,[L,1]) - repmat(xkvec',[1,L]);
    p = (0:(L-1))*L+(1:L);
    dnom(p) = 1.0f;
    denom = prod(dnom);
    num = repmat(x(i),[L,L]) - repmat(xkvec,[L,1]);
    num(p) = 1.0f;
    coeff(i,:) = prod(num,2)'./denom;
  end
  
% Calculate a Lagrange mapping from one set of coordinates to another.  Assumes
% that the set s_actual is monotonically increasing or decreasing.  Treats the
% boundary conditions as though they were constant.
function [wmap, wcoef] = get_inverse_map(s_actual, s_desired, order)
  s_actual = s_actual(:)';
  s_desired = s_desired(:);
  dmat = repmat(s_actual,[length(s_desired),1]) - repmat(s_desired,[1,length(s_actual)]);
  [mindist,mincol] = min(abs(dmat),[],2);
  % mincol is the element in s_actual closest to each element in s_desired
  % now we take +/- order channels
  mincol_mat = repmat(mincol,[1,2*order+1]) + repmat(-order:order,[length(s_desired),1]);
  % Wrap the columns...
  mincol_mat = mod(mincol_mat - 1,length(s_actual)) + 1;
  % Get the residual error values
  s_actuals = reshape(s_actual(mincol_mat(:)),size(mincol_mat));
  % Get the lagrange coefficients
  wcoef = get_lagrange_coeff(s_actuals,s_desired);
  wmap = mincol_mat;
  
% function [map_inplane, map_delay, P] = 
%                  calcfan2par(cfg,output_svals,delbeta,rad_order,beta_order)
% Calculate the mapping f0r fan2par rebinning f0r a fan beam
% geometry with fairly arbitrary source and detector coordinates
%  cfg = standard acq. config (2D info only is used) focal offset OK
%        fields used: x0, y0, xds, yds
%  output_svals = vector of parallel beam sampling locations
%  delbeta = delta beta of original data (also f0r the resampled data)
%  rad_order = order of interpolation to use f0r radial interpolation
%  beta_order = order of interpolation to use f0r beta interpolation
%  P is the window size to use on the output (i.e, need current view +/- P views)
function [map_inplane, map_delay, P, output_svals] = calcfan2par(cfg,delbeta,rad_order,beta_order)
% Get the number of detector channels (cols)
  nrdetcols = length(cfg.xds);
  y0mat = repmat([cfg.x0;cfg.y0],[1,nrdetcols]);
  xdmat = [cfg.xds(:)';cfg.yds(:)'];
  epsmat = xdmat - y0mat;
  % Calculate the point on each ray that is closest to the origin
  coeff = sum(epsmat.*y0mat)./sum(epsmat.^2);
  rstarmat = y0mat - repmat(coeff,[2,1]).*epsmat;
  % These are the parallel beam angles
  theta = atan2(rstarmat(1,:),rstarmat(2,:));
  % The absolute value of s
  s_abs = sqrt(sum(rstarmat.^2));
  % Fix up the angles and s values so that the angles are positive
  s_vals = s_abs;
  s_vals(theta<0) = -s_vals(theta<0);
  % Get the appropriate linear span
  [a,b] = min(s_abs);
  center_sval = s_vals(b);
  del_sval = s_vals(b) - s_vals(b-1);
  s_min = min(s_vals);
  s_max = max(s_vals);
  ndx_offset = floor((s_min - center_sval)/del_sval);
  s_offset = center_sval + del_sval*ndx_offset;
  ndx_maxs = ceil((s_max - center_sval)/del_sval);
  s_count = ndx_maxs - ndx_offset + 1;
  output_svals = s_offset + (0:(s_count-1))*del_sval;
  theta(theta<0) = pi + theta(theta<0);
  % Now, we have to get the detector-column mapping function
  [map,coef] = get_inverse_map(s_vals,output_svals,rad_order);
  map_inplane.map = map;
  map_inplane.coef = coef;
  % Also the delta theta map (easy one)
  % First calculate how far back and forward we need to go
  shift = pi/2-theta;
  P = ceil(max(abs(shift/delbeta))) + beta_order + 2;
  % There is a different shift for each column of the detector
  [map,coef] = get_inverse_map(-P:P,-shift/delbeta,beta_order);
  map_delay.map = map;
  map_delay.coef = coef;

  
% function numviews_out = fan2par_helical_file(filename_in,filename_out,cfg,delbeta,
%                                              radorder,betaorder_cfg,numviews)
%
% Apply fan2par rebinning to a file - designed to be quite flexible, allows for
% arbitrary detector geometries, source positions, etc.  Unlike an axial scan
% we have to throw away views at the start and end of the acquisition - hence
% the distinction of "helical".  
%
% Inputs:
%   filename_in  - must be float32s, no header, stored detector row at a time sequentially
%   filename_out - output filename - same format as input
%   cfg          - standard CTTB config struct
%   delbeta      - the view spacing (in radians)
%   radorder     - order of interpolation to use in the radial direction
%   betaorder    - order of interpolation to use in the angular direction
%   viewcount    - number of views in the file
%
% Outputs:
%   numviews_out - number of views in the output file
%
function numviews_out = fan2par_helical_file(filename_in,filename_out,cfg,delbeta,radorder,betaorder,viewcount)
  printf('Computing fan2par maps...\n');
  [map_inplane, map_delay, P, output_svals] = calcfan2par(cfg,delbeta,radorder,betaorder);
  % Set up the holding buffer
  printf('Initializing stream...\n');
  fp_in = fopen(filename_in,'rb');
  holdbuf = fread(fp_in,[cfg.detUCount,cfg.detVCount,2*P+1],'float');
  fp_out = fopen(filename_out,'wb');
  % Set up the buffer to hold the angularly interpolated view
  qview = zeros(cfg.detUCount,cfg.detVCount);
  outblock = zeros(cfg.detVCount,length(output_svals),viewcount);
  numviews_out = int32(viewcount - 2*P);
  for i = 1:numviews_out
    printf('Rebinning view %d...\r',i);
    % Apply viewshift to it
    viewshift(cfg.detUCount,cfg.detVCount,2*P+1,size(map_delay.map,2),...
              holdbuf,map_delay.map,map_delay.coef,qview);
    % Rebin in the in-plane direction
    rqview = apply_interp_map(qview',map_inplane.map,map_inplane.coef);
    fwrite(fp_out,float(rqview'));
    % Rotate the holdbuf
    holdbuf = circshift(holdbuf,[0,0,-1]);
    % Read in a new view
    if (i < numviews_out)
      holdbuf(:,:,end) = fread(fp_in,[cfg.detUCount,cfg.detVCount],'float');
    end
  end
  fclose(fp_in);
  fclose(fp_out);
  printf('\ndone.\n');
  
function h = getflatramp(cfg)
  a = cfg.detUSize;
  n = cfg.detUCount*2-1;
  m = int32(2^ceil(log2(n)));
  h = zeros(1,m);
  h(1) = 1/(8*a^2);
  p = 1:2:(m/2);
  h(p+1) = -1./(2*p.^2*pi^2*a^2);
  h(end:-1:(m/2+1)) = h(2:(m/2+1));
  h = h*a;

% function rampfilter_flat_file(filename_in,filename_out,cfg,numviews)
%
% Apply a ramp filter to a file - This is the classic ramp filter, not one of the
% GEHC kernels.  Uses the flat ramp (correct for parallel beam and flat fans)
%
% Inputs:
%   filename_in  - name of the input file containing the views - float32, no header
%                  views stored detector row at a time sequentially
%   filename_out - name of the output file
%   cfg          - standard CTTB config struct
%   numviews     - number of views in the file
function rampfilter_flat_file(filename_in,filename_out,cfg,numviews,delbeta)
  printf('Computing fan2par maps...\n');
  [map_inplane, map_delay, P, output_svals] = calcfan2par(cfg,delbeta,4,4);
  ncols = size(map_inplane.map,1);
  fp_in = fopen(filename_in,'rb');
  fp_out = fopen(filename_out,'wb');
  printf('Retrieving ramp filter...\n');
  ramp = float(getflatramp(cfg));
  for i = 1:numviews
    printf('Filtering view %d...\r',i);
    view = fread(fp_in,[ncols,cfg.detVCount],'float')';
    view = filtrows(view,ramp);
    fwrite(fp_out,float(view'));
  end
  fclose(fp_out);
  fclose(fp_in);
  printf('\ndone.\n');
  
function coscone = coneweight(numrows,sid,zdetatiso)
  heightz = zdetatiso*((0:(numrows-1)) - numrows/2 + 0.5);
  coscone = sid/sqrt(heightz.^2+sid^2);

function wght = viewweight_overscan(num_views,center_view)
  view_num = 0:(num_views-1);
  dBeta = 2*pi/984;
  fOverScan = 0.825*pi;
  
  betaView = (view_num - center_view)*dBeta;
  r1 = (betaView >= -(pi+fOverScan)) & (betaView <= -(2*pi-fOverScan));
  r2 = (betaView > -(2*pi-fOverScan)) & (betaView <= 2*pi-fOverScan);
  r3 = (betaView > 2*pi-fOverScan) & (betaView <= pi+fOverScan);
  
  wght = betaView*0.0;
  wght(r1) = 0.66667*(betaView(r1)+pi+fOverScan)/(2*fOverScan-pi);
  wght(r2) = 0.66666;
  wght(r3) = 0.66667*(1.0 - (betaView(r3)-2*pi+fOverScan)/(2*fOverScan-pi));
  wght = wght*0.5;

function wght = viewweight_twopi(num_views,center_view)
  view_num = 0:(num_views-1);
  dBeta = 2*pi/984;
  betaview = (view_num - center_view)*dBeta;
  betaturnpoint = 0.55*pi; % Only valid pitch 63
  r1 = (betaview >= -pi) & (betaview <= -betaturnpoint);
  r2 = (betaview >= -betaturnpoint) & (betaview <= -(pi-betaturnpoint));
  r3 = (betaview > -(pi-betaturnpoint)) & (betaview <= 0);
  r4 = (betaview > 0) & (betaview < (pi-betaturnpoint));
  r5 = (betaview > (pi-betaturnpoint)) & (betaview <= betaturnpoint);
  r6 = (betaview > betaturnpoint) & (betaview <= pi);
  wght = betaview*0.0;
  wght(r1) = (betaview(r1)+pi)/(pi - betaturnpoint);
  wght(r2) = 1.0;
  wght(r3) = 1.0 + (betaview(r3) + pi - betaturnpoint)/(pi-betaturnpoint);
  wght(r4) = 2.0 - betaview(r4)/(pi - betaturnpoint);
  wght(r5) = 1.0;
  wght(r6) = 1.0 - (betaview(r6) - betaturnpoint)/(pi - betaturnpoint);
  wght = wght*0.5;
  
% function numviews_out = view_weight_file(filename_in,filename_out,cfg,numviews,cv,weight)
%
% Apply view weighting to a file.  This version supports two weight types,
% overscan and 2-pi weighting.  You select the weighting you want by setting
% the weight argument appropriately.  
%
% Inputs:
%   filename_in  - name of the input file containing the views - float32, no header
%                  views stored detector row at a time sequentially
%   filename_out - name of the output file
%   cfg          - standard CTTB config struct
%   numviews     - number of views in the file
%   cv           - the center view to use in the weighting
%   weight       - either the string 'overscan' or 'twopi' depending on which 
%                  weight function you wish to use.
function numviews_out = view_weight_file(filename_in,filename_out,cfg,numviews,cv,weight,delbeta)
  printf('Computing weight factors...\n');
  vweight = feval(['viewweight_',weight],numviews,cv);
  zweight = coneweight(cfg.detVCount,cfg.sid,cfg.detVSize*cfg.sid/cfg.sdd);
  printf('Computing fan2par maps...\n');
  [map_inplane, map_delay, P, output_svals] = calcfan2par(cfg,delbeta,4,4);
  ncols = size(map_inplane.map,1);
  fp_in = fopen(filename_in,'rb');
  fp_out = fopen(filename_out,'wb');
  % Find the subset of view that are non-zero weighted
  nonz = find(vweight);
  % Seek to the centerview
  ccv = (length(nonz)+1)/2;
  skip_count = max(0,cv - ccv);
  fseek(fp_in,ncols*cfg.detVCount*4*skip_count,'bof');
  for i = nonz
    printf('view %d...\r',i);
    view = fread(fp_in,[ncols,cfg.detVCount],'float')';
    view = view.*repmat(zweight'*vweight(i),[1,ncols]);
    fwrite(fp_out,float(view'));
  end
  fclose(fp_out);
  fclose(fp_in);
  printf('\ndone.\n');
  numviews_out = length(nonz);

% function img = view_weight_file(filename_in,cfg,numviews,cv)
%
% Backproject the data onto an image grid.  
%
% Inputs:
%   filename_in  - name of the input file containing the views - float32, no header
%                  views stored detector row at a time sequentially
%   cfg          - standard CTTB config struct
%   delbeta      - the angular spacing between views
%   pitch        - helical pitch (in mm/rotation)
%   numviews     - number of views in the file
%   cv           - the center view to use in the weighting
function img = bp_semicone_flat_file(filename_in,cfg,delbeta,pitch,numviews,cv)
  printf('Computing fan2par maps...\n');
  [map_inplane, map_delay, P, output_svals] = calcfan2par(cfg,delbeta,4,4);
  ncols = size(map_inplane.map,1);
  fp_in = fopen(filename_in,'rb');
  % Set up the image
  img = float(zeros(cfg.recon_size));
  xypixelsize = cfg.recon_fov/cfg.recon_size;
  rowndx = min(cfg.detVCount,max(1,(1:cfg.detVCount)));
%  rowndx = min(cfg.detVCount,max(1,(1:cfg.detVCount)+cfg.recon_zshift));
  for i = 1:numviews
    printf('view %d... zpos = %f\r',i,(i-numviews/2)*delbeta*pitch/(2*pi));
    view = fliplr(fread(fp_in,[ncols,cfg.detVCount],'float'));
    view = view(:,rowndx);
    pd3dbpsemi_flat_c(view,img,cfg.recon_size,cfg.recon_xcenter,cfg.recon_ycenter,xypixelsize,...
                      length(output_svals),cfg.detVCount,cfg.sid,cfg.sdd,...
                      cfg.detVSize,cfg.detVCenter,min(output_svals),...
                      output_svals(2)-output_svals(1),...
                      -(cv-numviews/2-1+i)*delbeta,(i-numviews/2)*delbeta*pitch/(2*pi),pitch);
%    if (mod(i,10) == 0), image(img); end;
    %    image(img);
  end
  fclose(fp_in);
  printf('\ndone.\n');
  

