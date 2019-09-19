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

def construct_percentage_bar(value, upper_limit, char_length, show_value = False):
    redline = upper_limit
    limit = 9500
    percentage = float(value) / limit
    res = '['
    for i in range(0, char_length):
        if i/char_length < percentage:
            res += '|'
        else:
            res += ' '
    if show_value:
        res += '] {}'.format(value)
    else:
        res += '] {0:.0%}'.format(percentage)
    return res

def row_to_text(row):
    row[0] = construct_percentage_bar(row[0], 100, 20)
    row[2] = construct_percentage_bar(row[2], 9500, 20, True)
    return '''
    Throttle              {0}
    Brake                 {1}
    Rev                   {2}

    Speed                 {3}
    Coolant temp          {4}
    Intake Air Temp       {5}

    Lap                   {6}
    Predicted Lap Time    {7}
    Predicted vs Best Lap {8}
    '''.format(*row)
 
    #lap = row[2]
    #speed = row[11]
    #if len(row) < 22:
    #    rev = 0
    #else:
    #    rev = row[21]
    #return 'Lap:\t{}\nSpeed:\t{}\nRev:\t{}\n'.format(lap, speed, construct_percentage_bar(rev, 9500, 50))

def render_to_image_seq(width, height):
    # get line count 
    row_count = 0
    with open(input_data) as f:
        row_count = sum(1 for line in f)

    csv_file = open(input_data)

    csv_reader = csv.reader(csv_file, delimiter=',')

   # Hard coded items
    items_to_visit = [
            'Throttle Position (%) *OBD',
            'Brake (calculated)',
            'Engine Speed (RPM) *OBD', 
            'Vehicle Speed (km/h) *OBD',
            'Engine Coolant Temp (C) *OBD',
            'Intake Air Temp (C) *OBD',

            'Lap',
            'Predicted Lap Time',
            'Predicted vs Best Lap',
            ]
    item_indexes = [];

    hearder_read = False
    header = []
    for index, row in enumerate(csv_reader):
        fontsize = 35
        if row[0][0] == '#':
            continue
        if not hearder_read:
            header = row
            hearder_read = True
            for v in items_to_visit:
                item_indexes.append(row.index(v))
        else:
            lean_row = []
            for i in item_indexes:
                if i < len(row):
                    lean_row.append(row[i])
                else:
                    lean_row.append(-1)
            #print(row_to_text(lean_row), flush=True)
            render_image(row_to_text(row), index)
            print("Progress {0:.0%}".format(float(index)/row_count))

if __name__ == "__main__":
    render_to_image_seq(1920, 1080)
