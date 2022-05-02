import serial
import random
import sys

board=[i for i in range(0,9)]
player, computer = '',''

serialPort = serial.Serial('COM5', 115200, timeout=5)

# Corners, Center and Others, respectively
moves=((1,7,3,9),(5,),(2,4,6,8))
# Winner combinations
winners=((0,1,2),(3,4,5),(6,7,8),(0,3,6),(1,4,7),(2,5,8),(0,4,8),(2,4,6))
# Table
tab=range(1,10)

def print_board():
    x=1
    for i in board:
        end = ' | '
        if x%3 == 0:
            end = ' \n'
            if i != 1: end+='---------\n'
        char=' '
        if i in ('X','O'): char=i
        x+=1
        print(char,end=end)
        
def select_char():
    chars=('X','O')
    if random.randint(0,1) == 0:
        return chars[::-1]
    return chars

def can_move(brd, player, move):
    if move in tab and brd[move-1] == move-1:
        return True
    return False

def can_win(brd, player, move):
    places=[]
    x=0
    for i in brd:
        if i == player: places.append(x)
        x+=1
    win=True
    for tup in winners:
        win=True
        for ix in tup:
            if brd[ix] != player:
                win=False
                break
        if win == True:
            break
    return win

def make_move(brd, player, move, undo=False):
    if can_move(brd, player, move):
        brd[move-1] = player
        win=can_win(brd, player, move)
        if undo:
            brd[move-1] = move-1
        return (True, win)
    return (False, False)

# AI goes here
def computer_move():
    move=-1
    # If I can win, others don't matter.
    for i in range(1,10):
        if make_move(board, computer, i, True)[1]:
            move=i
            # print(move)
            var = str(move)
            serialPort.write(var.encode())
            break
    if move == -1:
        # If player can win, block him.
        for i in range(1,10):
            if make_move(board, player, i, True)[1]:
                move=i
                # print(move)
                var = str(move)
                serialPort.write(var.encode())
                break
    if move == -1:
        # Otherwise, try to take one of desired places.
        for tup in moves:
            for mv in tup:
                if move == -1 and can_move(board, computer, mv):
                    move=mv
                    # print(move)
                    var = str(move)
                    serialPort.write(var.encode())
                    break
    return make_move(board, computer, move)

def space_exist():
    return board.count('X') + board.count('O') != 9

player, computer = select_char()


i=1
pas = 0

serialString = "" 

numar = str(random.randint(1, 9))
data = numar.encode()
i=0
serialPort.write(data)
player, computer = select_char()
print('Player is [%s] and computer is [%s]' % (player, computer))
while 1:

    if(1):
        print_board()
        serialString = serialPort.readline()

        
        # entire_word = serialString.decode('Ascii')
            # print(entire_word)
            # move = int(entire_word[1])


        entire_word = serialString.decode('Ascii')
        print(entire_word)
        move = int(entire_word[1])
        print(move)
        make_move(board, player, move)
        computer_move()
        


        
