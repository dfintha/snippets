% leftpad.cpp
% The infamous leftpad function, rewritten in MATLAB.
% Author: Dénes Fintha
% Year: 2022
% ----------------------------------------------------------------------- %

% Implementation

function result = leftpad(string, length, filler)
    old_length = strlength(string);
    if length <= old_length
        new_length = old_length;
    else
        new_length = length;
    end
    pad_length = new_length - old_length;

    if ~isa(filler, "string")
        filler = num2str(filler);
    end
    filler = convertStringsToChars(filler);

    pad = "";
    for i = 1:pad_length
        pad = append(pad, filler(1));
    end
    
    result = strcat(pad, string);
end
