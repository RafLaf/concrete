<p align="center">
<!-- product name logo -->
  <img width=600 src="https://user-images.githubusercontent.com/5758427/231207493-62676aea-4cb9-4bb4-92b0-20309c8a933a.png">
</p>
<hr/>

<p align="center">
  <a href="https://docs.zama.ai/concrete"> 📒 Documentation</a> | <a href="https://zama.ai/community"> 💛 Community support</a> | <a href="https://github.com/zama-ai/awesome-zama"> 📚 FHE resources by Zama</a>
</p>

<p align="center">
  <a href="https://github.com/zama-ai/concrete/releases"><img src="https://img.shields.io/github/v/release/zama-ai/concrete?style=flat-square"></a>
  <a href="https://github.com/zama-ai/concrete/blob/main/LICENSE.txt"><img src="https://img.shields.io/badge/License-BSD--3--Clause--Clear-%23ffb243?style=flat-square"></a>
  <a href="https://github.com/zama-ai/bounty-program"><img src="https://img.shields.io/badge/Contribute-Zama%20Bounty%20Program-%23ffd208?style=flat-square"></a>
</p>

## About

### What is Concrete
**Concrete** is an open-source FHE Compiler that simplifies the use of fully homomorphic encryption (FHE). Concrete framework contains a TFHE Compiler based on LLVM, making writing FHE programs an easy task for developers. 

Fully Homomorphic Encryption (FHE) enables performing computations on encrypted data directly without the need to decrypt it first. FHE allows developers to build services that ensure privacy for all users. FHE is also an excellent solution against data breaches as everything is performed on encrypted data. Even if the server is compromised, no sensitive data will be leaked.

Concrete is a versatile library that can be used for a variety of purposes. For instance, [Concrete ML](https://github.com/zama-ai/concrete-ml) is built on top of Concrete to simplify Machine-Learning oriented use cases.
<br></br>


## Table of Contents
- **[Getting Started](#getting-started)**
   - [Installation](#installation)
   - [A simple example](#a-simple-example)
- **[Resources](#resources)**
   - [Concrete deep dive](#concrete-deep-dive)
   - [Tutorials](#tutorials)
   - [Documentation](#documentation)
- **[Working with Concrete](#working-with-concrete)**
   - [Citations](#citations)
   - [Contributing](#contributing)
   - [License](#license)
- **[Support](#support)**
<br></br>

## Getting Started

###  Installation
Depending on your OS, Concrete may be installed with Docker or with pip:

|               OS / HW                       | Available on Docker | Available on PyPI |
| :-----------------------------------------: | :-----------------: | :--------------: |
|                Linux                        |         Yes         |       Yes        |
|               Windows                       |         Yes         |    Coming soon   |
|     Windows Subsystem for Linux             |         Yes         |       Yes        |
|            macOS 11+ (Intel)                |         Yes         |       Yes        |
| macOS 11+ (Apple Silicon: M1, M2, etc.)     |      Coming soon    |       Yes        |


#### Docker
The preferred way to install Concrete is through PyPI:

```shell
pip install -U pip wheel setuptools
pip install concrete-python
```

#### Pip
You can get the concrete-python docker image by pulling the latest docker image:

```shell
docker pull zamafhe/concrete-python:v2.0.0
```

*Find more detailed installation instructions in [this part of the documentation](https://docs.zama.ai/concrete/getting-started/installing)*

<p align="right">
  <a href="#about" > ↑ Back to top </a> 
</p>

### A simple example
To compute on encrypted data, you first need to define the function you want to compute, then compile it into a Concrete Circuit, which you can use to perform homomorphic evaluation.
Here is the full example:

```python
from concrete import fhe

def add(x, y):
    return x + y

compiler = fhe.Compiler(add, {"x": "encrypted", "y": "encrypted"})
inputset = [(2, 3), (0, 0), (1, 6), (7, 7), (7, 1), (3, 2), (6, 1), (1, 7), (4, 5), (5, 4)]

print(f"Compiling...")
circuit = compiler.compile(inputset)

print(f"Generating keys...")
circuit.keygen()

examples = [(3, 4), (1, 2), (7, 7), (0, 0)]
for example in examples:
    encrypted_example = circuit.encrypt(*example)
    encrypted_result = circuit.run(encrypted_example)
    result = circuit.decrypt(encrypted_result)
    print(f"Evaluation of {' + '.join(map(str, example))} homomorphically = {result}")
```

Or if you have a simple function that you can decorate, and you don't care about explicit steps of key generation, encryption, evaluation and decryption:

```python
from concrete import fhe

@fhe.compiler({"x": "encrypted", "y": "encrypted"})
def add(x, y):
    return x + y

inputset = [(2, 3), (0, 0), (1, 6), (7, 7), (7, 1), (3, 2), (6, 1), (1, 7), (4, 5), (5, 4)]

print(f"Compiling...")
circuit = add.compile(inputset)

examples = [(3, 4), (1, 2), (7, 7), (0, 0)]
for example in examples:
    result = circuit.encrypt_run_decrypt(*example)
    print(f"Evaluation of {' + '.join(map(str, example))} homomorphically = {result}")
```
*This example is explained in more detail [in this part of the documentation](https://docs.zama.ai/concrete/getting-started/quick_start).*

<p align="right">
  <a href="#about" > ↑ Back to top </a> 
</p>

## Resources 

### Concrete deep dive
- [Part I: Concrete, Zama's Fully Homomorphic Compiler](https://www.zama.ai/post/zama-concrete-fully-homomorphic-encryption-compiler)
- [Part II: The Architecture of Concrete, Zama's Fully Homomorphic Encryption Compiler Leveraging MLIR](https://www.zama.ai/post/the-architecture-of-concrete-zama-fully-homomorphic-encryption-compiler-leveraging-mlir)
<br></br>

### Tutorials
- [[Video tutorial] Dive into Concrete - Zama's Fully Homomorphic Encryption Compiler](https://www.zama.ai/post/video-tutorial-dive-into-concrete-zamas-fully-homomorphic-encryption-compiler)
- [[Video tutorial] How To Get Started With Concrete - Zama's Fully Homomorphic Encryption Compiler](https://www.zama.ai/post/how-to-started-with-concrete-zama-fully-homomorphic-encryption-compiler)
- [The Encrypted Game of Life in Python Using Concrete](https://www.zama.ai/post/the-encrypted-game-of-life-using-concrete-python)
- [Encrypted Key-value Database Using Homomorphic Encryption](https://www.zama.ai/post/encrypted-key-value-database-using-homomorphic-encryption)
- [SHA-256 Implementation Using Concrete](https://github.com/zama-ai/concrete/blob/main/docs/application-tutorial/sha256.ipynb)

*Explore more useful resources in [Concrete tutorials](https://docs.zama.ai/concrete/tutorials/) and [Awesome Zama repo](https://github.com/zama-ai/awesome-zama). If you have built awesome projects using Concrete, please let us know and we will be happy to showcase them here!*



### Documentation

Full, comprehensive documentation is available at [https://docs.zama.ai/concrete](https://docs.zama.ai/concrete).

<p align="right">
  <a href="#about" > ↑ Back to top </a> 
</p>



## Working with Concrete

### Citations
To cite Concrete in academic papers, please use the following entry:

```text
@Misc{Concrete,
  title={{Concrete: TFHE Compiler that converts python programs into FHE equivalent}},
  author={Zama},
  year={2022},
  note={\url{https://github.com/zama-ai/concrete}},
}
```
### Contributing 

There are two ways to contribute to Concrete. You can:
- [Open issues](https://github.com/zama-ai/concrete/issues/new/choose) to report bugs and typos, or to suggest new ideas
- Request to become an official contributor by emailing hello@zama.ai.

Becoming an approved contributor involves signing our Contributor License Agreement (CLA)). Only approved contributors can send pull requests (PRs), so get in touch before you do!

Additionally, you can contribute to advancing the FHE space with Zama by participating in our [Bounty Program and Grant Programs](https://github.com/zama-ai/bounty-and-grant-program)! 
<br></br>
 
### License
This software is distributed under the **BSD-3-Clause-Clear**. [Check for more details]( https://github.com/zama-ai/concrete/blob/main/LICENSE.txt)  

If you have any questions, please contact us at hello@zama.ai.
<p align="right">
  <a href="#about" > ↑ Back to top </a> 
</p>

## Support

<a target="_blank" href="https://community.zama.ai">
  <img src="https://github.com/zama-ai/concrete/assets/157474013/3b24f84e-416c-44c2-9f93-42ccb04f0f33">
</a>

🌟 If you find this project helpful or interesting, please consider giving it a star on Github ! Your support helps to grow the community and motivates further development. 


<p align="right">
  <a href="#about" > ↑ Back to top </a> 
</p>

