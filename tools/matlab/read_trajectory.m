function p = read_trajectory(filename)
%
% p = read_trajectory(filename)
%
% reads trajectory from file filename in standard format
%
% 2016 Bernd Pfrommer
%
%% --- inputs
% filename      name of file with pose information
%% --- outputs
% p             trajectory
%% --- example usage
%
% p = read_trajectory('../../data/ground_truth/as/pose.txt');
%
    h     = textread(filename);
    n     = size(h,1);
    omega = zeros(n, 4);
    t     = zeros(n, 3);
    for i=1:n
        omega(i,:) = vrrotmat2vec([h(i,2:4); h(i,6:8); h(i, 10:12)]);
        t(i, :)    = [h(i,5), h(i,9), h(i,13)];
    end

    p.tsraw = h(:,1);
    p.ts    = p.tsraw;
    p.trans = t;
    p.omega = omega;
    p.nobs  = ones(n, 1);
    p.name  = filename;
end
