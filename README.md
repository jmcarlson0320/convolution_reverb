# README
## CS410 Computers Sound and Music
## Term Project Spring 2020
## Professor: Bart Massey
## Convolution Reverb
Author: Joshua Carlson, jmc34@pdx.edu
Date: 6/5/2020

# Overview
This project implements a convolution reverb audio effect. It takes as input a
wave file, and either plays back the effected audio, or writes the effected
audio to a specified output file. This project includes three sample input
files, as well as 13 impulse responses representing interesting rooms,
cathedrals, and other special effects.

# Building and Running
This program was built and tested on Ubuntu Linux.
The following libraries are needed to run this program (terminal commands
provided for ubuntu users).
* Port Audio
  * 'sudo apt install portaudio19-dev'
* libsndfile
  * 'sudo apt install libsndfile1-dev'
* liquid-dsp
  * 'sudo apt install libliquid-dev'
* fftw
  * sudo apt install fftw-dev'

Build by running 'make' from the command line
Run using './reverb -l' to list the impuse responses.
Run using './reverb -i impulse_response input_file.wav' to apply reverb and
play the audio.
Run using './reverb -i impulse_response -o output_file.wav input_file.wav' to
apply reverb and write the result to a new wave file.

*Important*: this program only works with stereo 16-bit pcm wave files. Three
sample inputs are provided:

* 'dog.wav' is a long input of a dog barking.
* 'loop.wav' is a music clip with short percussive sounds.
* 'jojo_bartmassey.wav' is a clip of my daughter reciting the famous tagline :)

# Testing
I did not perform testing in this project as systematically as I would have
liked. I setup a framework for unit-testing, but the tests actually written
were more of a way to test drive the code as I worked on it. They can be ran
by using 'make test' followed by './tests'.

# Reflection
This project was *a bit* more challenging than I anticipated. And the problems
arose in the final weeks of working on it. From what we learned in class,
convolution is a straightforward process. However, when I got to the point of
implementing the convolution in my project, I realized that naively implementing
a convolution reverb is inhibitively inefficient. This is an O(N^2) algorithim
and even short audio signals and impulse responses are hundreds of thousands to
millions of samples long. Simply convolving one signal with the other will not
work except on the shortest signals. I needed to figure out a way to do this
efficiently or I would not be able to complete it.

Throughout this whole term I've been diving deep into digital signal
processing, and a lecture on the DFT mentioned the convolution property of the
Fourier Transform. The property that convolution in the time domain is equal to
multiplication in the frequency domain, and vice-versa. Because we have an
efficient way to go between the time and frequency domain by using the FFT,
I thought I could use this to my advantage. I began researching that more, and
it turns out that is exactly what is done. Long signals are transformed to the
frequency domain and multiplied, then transformed back to the time domain. This
is even sometimes called "fast convolution".

However, for long signals, simply performing an FFT on the entire signal
still wont work, because that still means a several hundred thousand or multi-
million point FFT. The way around that is by using the *Overlap-add* method.
This method breaks up the input signal into managable pieces, convolves them
with the impulse response, and puts them back together, while overlapping and
adding the tail that is generated by convolution.

This solved the problem of long inputs, but there was still the problem of
a long impulse response. Getting around that involved studying a diagram found
on a blog post at:
    [https://dvcs.w3.org/hg/audio/raw-file/tip/webaudio/convolution.html]
This post described also breaking up the impulse response, but it is not as
straight forward as breaking up the input signal. Each piece of the impulse
response is given to the a convolution engine that convolves everything given to
it with its specific impulse response. The input blocks are fed to these
engines by using a multi-tap ring buffer large enough to hold a number of
sample blocks equal to the number of IR slices. The convolvers get their input
samples out of the buffer at a specified delay.

I found a good article on a delay buffer:
    [https://www.embedded.com/a-multi-tap-software-delay-buffer/], and implemented
a simple version of it. Then worked on getting the segmented impulse response
algorithm working. The key to this was that everytime you load a new block of
samples into the buffer, you run *ALL* of the convolvers, and sum up *ALL* of
their outputs. This gives the complete convolution for that block of time
represented by that block of samples. The blocks are pushed through the
buffer as more blocks are added, and eventually every block gets fed into every
convolution engine. And it works!!!

    I am extremely satisfied with how it turned out, and am hooked on the
subject of DSP. There are still things to improve with the project though. I'd
like to get this going in real-time, and to do that, requires not treating every
IR slice the same way. For example, the first IR convolver should just use
direct convolution and a short slice of the IR. This cuts down on the latency.
Other things, are to fix inefficiencies in my algorithms. I did not have a
chance to really refine the process. But overall, for a first attempt, I am
happy with it.

