(* by Niklaus Wirth, 
   from "Algorithms + Data Structures = Programs", Prentice-Hall 1976 
   as cited in http://en.wikipedia.org/wiki/Eight_queens_puzzle *)
program eightqueen1(output);
 
var i : integer; q : boolean;
    a : array[ 1 .. 8] of boolean;
    b : array[ 2 .. 16] of boolean;
    c : array[ -7 .. 7] of boolean;
    x : array[ 1 .. 8] of integer;
 
procedure try( i : integer; var q : boolean);
    var j : integer;
    begin 
    j := 0;
    repeat 
        j := j + 1; 
        q := false;
        if a[ j] and b[ i + j] and c[ i - j] then
            begin 
            x[ i    ] := j;
            a[ j    ] := false; 
            b[ i + j] := false; 
            c[ i - j] := false;
            if i < 8 then
                begin
                try( i + 1, q);
                if not q then
                    begin 
                    a[ j] := true; 
                    b[ i + j] := true; 
                    c[ i - j] := true;
                    end
                end 
            else 
                q := true
            end
    until q or (j = 8);
    end;
 
begin
for i :=  1 to  8 do a[ i] := true;
for i :=  2 to 16 do b[ i] := true;
for i := -7 to  7 do c[ i] := true;
try( 1, q);
if q then
    for i := 1 to 8 do write( x[ i]:4);
writeln
end.
