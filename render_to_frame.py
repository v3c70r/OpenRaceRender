import csv

from PIL import Image, ImageDraw, ImageFont
input_data = './raw/Log-20190902-175620 Segment.csv'

def render_image(text, index, output_folder = 'output'):
    image = Image.new("RGBA", (1920, 1080), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    fontsize = 35
    font = ImageFont.truetype('Roboto/Roboto-Bold.ttf', fontsize)

    w, h = font.getsize(text)
    
    draw.text((10,10), text, fill='white', font=font)
    image.save('{}/{:0>5d}.png'.format(output_folder, index))

def construct_percentage_bar(value, upper_limit, char_length):
    redline = upper_limit
    limit = 9500
    percentage = float(value) / limit
    res = '['
    for i in range(0, char_length):
        if i/char_length < percentage:
            res += '|'
        else:
            res += ' '
    res += '] {0:.0%}'.format(percentage)
    return res

def row_to_text(row):
    '''
    Throttle [||||||||||      ]  50%
    Brake    [||||||||||      ]  50%
    Rev      [||||||          ]  RPM

    Speed    
    Gear     
    Lap
    Position  x, y ,z
    '''
    lap = row[2]
    speed = row[11]
    if len(row) < 22:
        rev = 0
    else:
        rev = row[21]
    return 'Lap:\t{}\nSpeed:\t{}\nRev:\t{}\n'.format(lap, speed, construct_percentage_bar(rev, 9500, 50))

def render_to_image_seq(width, height):
    csv_file = open(input_data)

    csv_reader = csv.reader(csv_file, delimiter=',')


    hearder_read = False
    header = []
    for index, row in enumerate(csv_reader):
        fontsize = 35
        if row[0][0] == '#':
            continue
        if not hearder_read:
            header = row
            hearder_read = True
        else:
            print(row_to_text(row), flush=True)
            #render_image(row_to_text(row), index)

    print(header)

if __name__ == "__main__":
    render_to_image_seq(1920, 1080)
