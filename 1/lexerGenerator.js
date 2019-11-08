const readline = require('readline');
const fs = require('fs');

const myInterface = readline.createInterface({
    input: fs.createReadStream('./lexer.orig.cpp')
});
const dest = fs.createWriteStream('./lexer.l');

let part = 0;
myInterface.on('line', function (line) {
    if (line.includes('// HEAD')) {
        part = 1;
        dest.write('%{\n');
    } else if (line.includes('// BODY')) {
        part = 2;
        dest.write('%}\n');
        dest.write(`%option nodefault\n%option noyywrap\n%option c++\n%option yyclass="Lexer"\n%option prefix="MiniC_"\n`);
        dest.write('\n%x COMMENT_BLOCK\n');
        dest.write('\n%%\n');
    } else if (line.includes('// TAIL')) {
        part = 3;
        dest.write('%%\n\n');
    } else {
        switch (part) {
            case 1: {
                if (!line.match(/^\s*$/)) {
                    dest.write('    ' + line + '\n');
                }
                break;
            }
            case 2: {
                if (line.includes('/// %')) {
                    dest.write(line.replace(/.*\/\/\/\s/, '') + '\n');
                } else if (line.includes('/// ')) {
                    dest.write(line.replace(/.*\/\/\/\s/, '') + ' {\n');
                } else if (line.includes('///')) {
                    dest.write('}\n');
                } else if (line.includes('// return')) {
                    dest.write(line.replace(/.*\/\/\s/, '') + '\n');
                } else if (!line.includes('int test') && !line.includes('} // END BODY')) {
                    dest.write(line + '\n');
                }
                break;
            }
            case 3: {
                dest.write(line + '\n');
                break;
            }
        }
    }
});