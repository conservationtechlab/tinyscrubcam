import os
import json
import cv2
from PytorchWildlife.models import detection as pw_det

# Config
base_image_folder = '/home/conor/Downloads/Temp_Images/tinyscrubcam_project'
species_folders = ['cougar', 'bobcat', 'coyote']
max_images_per_species = 4000
batch_size = 8

# Load the MegaDetectorV6 model
detector = pw_det.MegaDetectorV6(version="MDV6-yolov10-c")

def chunk_list(lst, chunk_size):
    for i in range(0, len(lst), chunk_size):
        yield lst[i:i + chunk_size]

for species in species_folders:
    folder = os.path.join(base_image_folder, species)
    print(f"🔍 Processing folder: {folder}")

    # Collect paths to image files
    all_image_paths = [
        os.path.join(folder, f)
        for f in os.listdir(folder)
        if f.lower().endswith(('.jpg', '.jpeg', '.png'))
    ][:max_images_per_species]

    all_results = []

    # Process images in small chunks
    for chunk_paths in chunk_list(all_image_paths, batch_size):
        images = []
        valid_paths = []
        for path in chunk_paths:
            img = cv2.imread(path)
            if img is not None:
                images.append(img)
                valid_paths.append(path)

        if not images:
            continue

        # Run detection
        results = detector.batch_image_detection(images, batch_size=len(images))

        for item, path in zip(results, valid_paths):
            item['species'] = species
            item['image_path'] = path
            all_results.append(item)

        print(f"  ✅ Processed {len(valid_paths)} images... (total so far: {len(all_results)})")

    # Save results to JSON
    output_path = os.path.join(base_image_folder, f'mdv6_results_{species}.json')
    with open(output_path, 'w') as f:
        json.dump(all_results, f, indent=2)

    print(f"✅ Saved results for {species} to {output_path}")
