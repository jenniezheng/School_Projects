%one 1 is dih...
dihdah([1],.).
%two ones can be dih
dihdah([1,1], .).
%or two ones can be dah
dihdah([1,1], -).
%three or more 1's is dah...
dihdah([1,1,1|Tail],-) :- \+(member(0, Tail)).


% ^ is boundary between letters
% # is space between words

%one 0 separates dih from dah
dihdah([0],~).
%two 0's can separate dihdah
dihdah([0,0],~).
%or two 0's can separates letters
dihdah([0,0],^).
%three 0's separates letters
dihdah([0,0,0],^).
%four 0's separate letters
dihdah([0,0,0,0],^).
%five 0's can separate letters
dihdah([0,0,0,0,0],^).
%or five 0's can separate words
dihdah([0,0,0,0,0],#).
%six or more 0's separates words
dihdah([0,0,0,0,0,0|Tail],#):- \+(member(1, Tail)).

%If separator, remove it
removeLetterSeperator([~],[]).
%else keep it
removeLetterSeperator(X,X):-
    \+(member(~, X)).

%default nothing
signal_morse([], []).

%single morse code
signal_morse(Binary, Morse) :-
    dihdah(Binary, MorseUnclean),
    removeLetterSeperator([MorseUnclean],Morse).

%multiple morse codes, start with 0
signal_morse(Binary, Morse) :-
    append(HeadBinary, TailBinary, Binary),
    %TailBinary must start with 0
    once(append([0],_,TailBinary)),
    %HeadBinary must have no zeros and must be nonempty
    HeadBinary\==[],
    \+(member(0, HeadBinary)),
    %get translation
    dihdah(HeadBinary,Morse1),
    %recurse
    signal_morse(TailBinary, Morse2),
    %append together
    append([Morse1], Morse2, Morse).

%multiple morse codes, start with 1
signal_morse(Binary, Morse) :-
    append(HeadBinary, TailBinary, Binary),
    %TailBinary must start with 1
    once(append([1],_,TailBinary)),
    %HeadBinary must have no ones and be nonempty
    HeadBinary\==[],
    \+(member(1, HeadBinary)),
    %get translation
    dihdah(HeadBinary,Morse1Unclean),
    removeLetterSeperator([Morse1Unclean],Morse1),
    %recurse
    signal_morse(TailBinary, Morse2),
    %append together
    append(Morse1, Morse2, Morse).


morse(a, [.,-]).           % A
morse(b, [-,.,.,.]).       % B
morse(c, [-,.,-,.]).       % C
morse(d, [-,.,.]).     % D
morse(e, [.]).         % E
morse('e''', [.,.,-,.,.]). % É (accented E)
morse(f, [.,.,-,.]).       % F
morse(g, [-,-,.]).     % G
morse(h, [.,.,.,.]).       % H
morse(i, [.,.]).       % I
morse(j, [.,-,-,-]).       % J
morse(k, [-,.,-]).     % K or invitation to transmit
morse(l, [.,-,.,.]).       % L
morse(m, [-,-]).       % M
morse(n, [-,.]).       % N
morse(o, [-,-,-]).     % O
morse(p, [.,-,-,.]).       % P
morse(q, [-,-,.,-]).       % Q
morse(r, [.,-,.]).     % R
morse(s, [.,.,.]).     % S
morse(t, [-]).         % T
morse(u, [.,.,-]).     % U
morse(v, [.,.,.,-]).       % V
morse(w, [.,-,-]).     % W
morse(x, [-,.,.,-]).       % X or multiplication sign
morse(y, [-,.,-,-]).       % Y
morse(z, [-,-,.,.]).       % Z
morse(0, [-,-,-,-,-]).     % 0
morse(1, [.,-,-,-,-]).     % 1
morse(2, [.,.,-,-,-]).     % 2
morse(3, [.,.,.,-,-]).     % 3
morse(4, [.,.,.,.,-]).     % 4
morse(5, [.,.,.,.,.]).     % 5
morse(6, [-,.,.,.,.]).     % 6
morse(7, [-,-,.,.,.]).     % 7
morse(8, [-,-,-,.,.]).     % 8
morse(9, [-,-,-,-,.]).     % 9
morse(., [.,-,.,-,.,-]).   % . (period)
morse(',', [-,-,.,.,-,-]). % , (comma)
morse(:, [-,-,-,.,.,.]).   % : (colon or division sign)
morse(?, [.,.,-,-,.,.]).   % ? (question mark)
morse('''',[.,-,-,-,-,.]). % ' (apostrophe)
morse(-, [-,.,.,.,.,-]).   % - (hyphen or dash or subtraction sign)
morse(/, [-,.,.,-,.]).     % / (fraction bar or division sign)
morse('(', [-,.,-,-,.]).   % ( (left-hand bracket or parenthesis)
morse(')', [-,.,-,-,.,-]). % ) (right-hand bracket or parenthesis)
morse('"', [.,-,.,.,-,.]). % " (inverted commas or quotation marks)
morse(=, [-,.,.,.,-]).     % = (double hyphen)
morse(+, [.,-,.,-,.]).     % + (cross or addition sign)
morse(@, [.,-,-,.,-,.]).   % @ (commercial at)

% Error.
morse(error, [.,.,.,.,.,.,.,.]). % error - see below

% Prosigns.
morse(as, [.,-,.,.,.]).          % AS (wait A Second)
morse(ct, [-,.,-,.,-]).          % CT (starting signal, Copy This)
morse(sk, [.,.,.,-,.,-]).        % SK (end of work, Silent Key)
morse(sn, [.,.,.,-,.]).          % SN (understood, Sho' 'Nuff)

%read the letters of a word
signal_letters([],[]).
%one letter
signal_letters(Dihdahs, [Letter]) :-
    %translate
    morse(Letter,Dihdahs).

%multiple letters
signal_letters(Dihdahs, Letters) :-
    %check only single word
    \+(member(#, Dihdahs)),
    append(FirstLetterDihDah, [^|RestLettersDihDah], Dihdahs),
    %check FirstLetterDihDah has no ^ and is nonempty
    FirstLetterDihDah\==[],
    \+(member(^, FirstLetterDihDah)),
    %get translation of first
    morse(Letter,FirstLetterDihDah),
    %recurse
    signal_letters(RestLettersDihDah, RestLetters),
    %add letters together
    append([Letter],RestLetters,Letters).


%skip #
signal_words([#|Tail], Words):-
    signal_words(Tail, Words2),
    append([#],Words2,Words).

%only one word
signal_words(Dihdahs, Words):-
    signal_letters(Dihdahs, Words).

%multiple words
signal_words(Dihdahs, Words) :-
    append(FirstWordDihDah, [#|RestWordsDihDah], Dihdahs),
    %translate to letters
    FirstWordDihDah\==[],
    once(signal_letters(FirstWordDihDah, FirstWord)),
    %recurse
    signal_words(RestWordsDihDah, RestWords),
    %add letters together
    append(FirstWord,[#|RestWords],Words).



%check if word is anything + optional ####
onlySpaces([]).
onlySpaces([#|Rest]):-
    onlySpaces(Rest).

wordAndMaybeSpaces(Word):-
    \+(member(#,Word)).

wordAndMaybeSpaces(WordWithSpaces):-
    member(#,WordWithSpaces),
    append(Word, Spaces, WordWithSpaces),
    Word\==[],
    \+(member(#,Word)),
    onlySpaces(Spaces).

%if no more errors, just return
signal_errors(Morse,Morse):-
    \+(member(error,Morse)).

%if start with error, just add it to morse
signal_errors([error|Tail],Morse):-
    signal_errors(Tail,Morse2),
    append([error],Morse2,Morse).

%if start with #, just add it to morse
signal_errors([#|Tail],Morse):-
    member(error,Tail),
    signal_errors(Tail,Morse2),
    append([#],Morse2,Morse).

%if find single word, skip it
signal_errors(MorseWithErrors,Morse):-
    %Word, error, then others
    append(Word, [error|Others], MorseWithErrors),
    %no errors in Words and Words not empty
    \+(onlySpaces(Word)),
    \+(member(error,Word)),
    wordAndMaybeSpaces(Word),
    %recurse
    signal_errors(Others,Morse).


%if find error sequence, with multiple words
signal_errors(MorseWithErrors,Morse):-
    %Word, error, then others
    append(Words, [error|Others], MorseWithErrors),
    %no errors in Words and Words not empty
    Words\==[],
    \+(member(error,Words)),
    %If find this sequence, remove the last word
    append(WordsWithoutLast,[#|LastWord],Words),
    \+(onlySpaces(LastWord)),
    wordAndMaybeSpaces(LastWord),
    %recurse
    signal_errors(Others,Morse2),
    append(WordsWithoutLast,[#|Morse2],Morse).


signal_message(Binary, Morse) :-
    signal_morse(Binary, Dihdahs),
    signal_words(Dihdahs,MorseWithErrors),
    signal_errors(MorseWithErrors,Morse).

