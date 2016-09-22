classdef CustomVideoReader < handle
%
% 2016 Bernd Pfrommer
% GRASP lab
%
% This class wraps the matlab video reader, and adds the
% functionality of reading ordered png frames like a video.
%
% The frames need to all be in one directory, and named frame_0000.png
% In the same directory must be a file "timestamps.txt" that has the
% corresponding timestamps of the frames (in seconds).
%
    properties
        matlabReader
        lastTime
        frameNumber
        fileNames
        currentTimeStamp
        timeStamps
    end
    
    methods
        function obj = CustomVideoReader(filenames)
            obj.frameNumber = 0;
            obj.fileNames   = [];
            obj.timeStamps  = [];
            
            hasSlash = filenames(end) == '/';
            if hasSlash
                obj.read_timestamps(filenames);
                if isempty(obj.timeStamps)
                    error 'no timestamps provided!'
                end
                obj.read_filenames(filenames);
                obj.currentTimeStamp = obj.timeStamps(1);
            else
                obj.matlabReader = VideoReader(filenames);
                disp(sprintf('using matlab reader for file %s', filenames));
                obj.currentTimeStamp = obj.matlabReader.CurrentTime;
            end
        end
        
        function f = readFrame(obj, varargin)
            obj.frameNumber = obj.frameNumber + 1;
            skip = false;
            if length(varargin) > 0 & varargin{1} == 1
                skip = true;
            end

            if ~isempty(obj.matlabReader)
                obj.currentTimeStamp = obj.matlabReader.CurrentTime;
                f = readFrame(obj.matlabReader);
            else
                obj.currentTimeStamp = obj.timeStamps(obj.frameNumber);
                if skip
                    f = [];
                else 
                    if false
                        disp(sprintf('reading: %s tstamp %.5f', ...
                                     obj.fileNames{obj.frameNumber}, ...
                                     obj.currentTimeStamp));
                    end
                    f = imread(obj.fileNames{obj.frameNumber});
                end
            end
        end
        
        function read_timestamps(obj, fnames)
            tsname = [fnames,'timestamps.txt'];
            if exist(tsname, 'file') ~= 2
                error('cannot find timestamp file: %s', tsname);
            end
            obj.timeStamps = textread(tsname);
        end
        
        function read_filenames(obj, fnames)
            d    = dir([fnames,'frame_*.png']);
            path = fileparts(fnames);
            nframes = length(d);
            ntimestamps = length(obj.timeStamps);
            if nframes ~= ntimestamps
                disp(sprintf('nframes: %d vs ntimestamps: %d', nframes, ...
                             ntimestamps));
                error('number of frames and timestamps mismatch');
            end
            for fnum = 1:nframes
                fn = d(fnum).name;
                sname = textscan(fn, '%s','Delimiter','_.');
                tfn = str2num(sname{1}{2});
                if tfn ~= fnum
                    error('frame number %d does not match file name %s', ...
                          fnum, fn);
                end
                fname = [path, '/', fn];
                obj.fileNames{fnum} = fname;
            end
        end
        
        function f = hasFrame(obj)
            f = 0;
            if ~isempty(obj.matlabReader)
                f = hasFrame(obj.matlabReader);
            else
                if obj.frameNumber < length(obj.timeStamps)
                    f = 1;
                end
            end
        end
        
        function n = numberOfFrames(obj)
            if ~isempty(obj.matlabReader)
                n = Inf;
            else
                n = length(obj.timeStamps);
            end
        end
    end
end
