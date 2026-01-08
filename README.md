# My-Image-Classifier-CIFAR10
# Introduction
My very first Machine learning Project. I imported a CIFAR-10 dataset (10 image classes) and built a Convolutional Neural Network (CNN) model that achieved approximately 85% Accuracy.

# Motivation
In my Second year of Information Engineering I took a course "Introduction to Machine Learning". I fell in love with the subject as we discussed classical (traditional) ML methods like Linear Regression, Logistic Regression, Decision Trees and others. However, deep learning and neural networks still seemed like a mystery. So, I decided to start an Image Classifier Project using CIFAR-10 Dataset.

# The process
First of all, I used Jupyter notebook, imported useful libraries and dataset itself. I split the dataset into training, validation and testing. I scaled the images and afterwards, created an instance of a model and added several convolutional and dense neural layers. Then, I began training and tweaking several parameters until I could reach desired accuracy/precision. Except the cat part, (Checking the confusion matrix Accuracy of correctly classifying cat images were lower than other classes) I was satisfied with the results. Finally, I made a very simple and basic flask app to mimic a website.

# What I learned/ My thoughts
Through this project, I gained a solid understanding of how neural networks work and what happens behind the scenes in these frameworks. I experimented extensively, facing challenges like underfitting and overfitting. Using techniques such as data augmentation, adjusting filter parameters, and applying regularization, I was able to solve these issues. In the end, the model achieved almost 85% accuracy. (With more epochs and a GPU, I could have pushed this further.) However, my primary goal was not the highest accuracy possible, it was to gain knowledge and understanding of how these models work, identify potential problems, and apply critical thinking to solve them. Ultimately, I discovered that my interest in deep learning continues to grow.

