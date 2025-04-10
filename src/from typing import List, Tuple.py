from typing import List, Tuple

# Mapping from original order to desired order
original_order = ['m', 'l', 'k', 'j', 'i', 'h', 'g2', 'g1', 'f', 'e', 'd2', 'd1', 'c', 'b', 'a2', 'a1']
desired_order = ['a1', 'a2', 'g1', 'g2', 'd1', 'd2', 'f', 'i', 'b', 'e', 'l', 'c', 'h', 'j', 'm', 'k']

# Generate index mapping
index_map = [original_order.index(segment) for segment in desired_order]


def reorder_segments(segment_data: List[Tuple[str, List[int]]]) -> List[Tuple[str, List[int]]]:
    reordered_data = []
    for label, segments in segment_data:
        reordered_segments = [segments[i] for i in index_map]
        reordered_data.append((label, reordered_segments))
    return reordered_data


def print_segments(segment_data: List[Tuple[str, List[int]]]):
    for label, segments in segment_data:
        segments_str = ', '.join(map(str, segments))
        print(f'  {{ {segments_str} }}, //{label}')


# Example Input Data
input_data = [  
    ('ZERO',  [1,0,0,1,0,0,0,0,1,1,1,1,1,1,1,1]),
    ('ONE',   [0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0]),
    ('TWO',   [0,0,0,0,0,0,1,1,0,1,1,1,0,1,1,1]),
    ('THREE', [0,0,0,0,0,0,1,1,0,0,1,1,1,1,1,1]),
    ('FOUR',  [0,0,0,0,0,0,1,1,1,0,0,0,1,1,0,0]),
    ('FIVE',  [0,0,0,0,0,0,1,1,1,0,1,1,1,0,1,1]),
    ('SIX',   [0,0,0,0,0,0,1,1,1,1,1,1,1,0,1,1]),
    ('SEVEN', [0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1]),
    ('EIGHT', [0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1]),
    ('NINE',  [0,0,0,0,0,0,1,1,1,0,1,1,1,1,1,1]),
    ('LETTER A', [0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1]),
    ('LETTER B', [0,1,0,0,1,0,1,0,0,0,1,1,1,1,1,1]),
    ('LETTER C', [0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1]),
    ('LETTER D', [0,1,0,0,1,0,0,0,0,0,1,1,1,1,1,1]),
    ('LETTER E', [0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,1]),
    ('LETTER F', [0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1]),
    ('LETTER G', [0,0,0,0,0,0,1,0,1,1,1,1,1,0,1,1]),
    ('LETTER H', [0,0,0,0,0,0,1,1,1,1,0,0,1,1,0,0]),
    ('LETTER I', [0,1,0,0,1,0,0,0,0,0,1,1,0,0,1,1]),
    ('LETTER J', [0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0]),
    ('LETTER K', [0,0,1,1,0,0,0,1,1,1,0,0,0,0,0,0]),
    ('LETTER L', [0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0]),
    ('LETTER M', [0,0,0,1,0,1,0,0,1,1,0,0,1,1,0,0]),
    ('LETTER N', [0,0,1,0,0,1,0,0,1,1,0,0,1,1,0,0]),
    ('LETTER O', [0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1]),
    ('LETTER P', [0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,1]),
    ('LETTER Q', [0,0,1,0,0,0,0,0,1,1,1,1,1,1,1,1]),
    ('LETTER R', [0,0,1,0,0,0,1,1,1,1,0,0,0,1,1,1]),
    ('LETTER S', [0,0,0,0,0,0,1,1,1,0,1,1,1,0,1,1]),
    ('LETTER T', [0,1,0,0,1,0,0,0,0,0,0,0,0,0,1,1]),
    ('LETTER U', [0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0]),
    ('LETTER V', [1,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0]),
    ('LETTER W', [1,0,1,0,0,0,0,0,1,1,0,0,1,1,0,0]),
    ('LETTER X', [1,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0]),
    ('LETTER Y', [0,1,0,0,0,0,1,1,1,0,0,0,0,1,0,0]),
    ('LETTER Z', [1,0,0,1,0,0,0,0,0,0,1,1,0,0,1,1]),
]

# Reorder and print the segments
output_data = reorder_segments(input_data)
print_segments(output_data)
