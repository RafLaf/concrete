//! Module with the definition of the ServerKey.
//!
//! This module implements the generation of the server public key, together with all the
//! available homomorphic integer operations.
mod add;
mod add_parallel;
mod bitwise_op;
mod bitwise_op_parallel;
mod crt;
mod crt_parallel;
mod mul;
mod mul_parallel;
mod neg;
mod neg_parallel;
mod scalar_add;
mod scalar_add_parallel;
mod scalar_mul;
mod scalar_mul_parallel;
mod scalar_sub;
mod scalar_sub_parallel;
mod shift;
mod shift_parallel;
mod sub;
mod sub_parallel;
#[cfg(test)]
mod tests;
#[cfg(test)]
mod tests_parallel;

use crate::ciphertext::{Ciphertext, KeyId};
use crate::client_key::ClientKey;
use concrete_shortint::server_key::MaxDegree;
use serde::{Deserialize, Serialize};

/// Error returned when the carry buffer is full.
pub use concrete_shortint::CheckError;

/// A structure containing the server public key.
///
/// The server key is generated by the client and is meant to be published: the client
/// sends it to the server so it can compute homomorphic integer circuits.
#[derive(Serialize, Deserialize, Clone)]
pub struct ServerKey {
    pub key: concrete_shortint::server_key::ServerKey,
}

impl ServerKey {
    /// Generates a server key.
    ///
    /// # Example
    ///
    /// ```rust
    /// use concrete_integer::{ClientKey, ServerKey};
    /// use concrete_shortint::parameters::PARAM_MESSAGE_2_CARRY_2;
    /// //Large integer defined over 4 blocks
    /// let size = 4;
    ///
    /// // Generate the client key:
    /// let cks = ClientKey::new(PARAM_MESSAGE_2_CARRY_2, size);
    ///
    /// // Generate the server key:
    /// let sks = ServerKey::new(&cks);
    /// ```
    pub fn new(cks: &ClientKey) -> ServerKey {
        // It should remain just enough space to add a carry
        let max =
            (cks.key.parameters.message_modulus.0 - 1) * cks.key.parameters.carry_modulus.0 - 1;

        let sks =
            concrete_shortint::server_key::ServerKey::new_with_max_degree(&cks.key, MaxDegree(max));

        ServerKey { key: sks }
    }

    /// Creates a ServerKey from an already generated shortint::ServerKey.
    ///
    /// # Example
    ///
    /// ```rust
    /// use concrete_integer::{ClientKey, ServerKey};
    /// use concrete_shortint::parameters::PARAM_MESSAGE_2_CARRY_2;
    ///
    /// let size = 4;
    ///
    /// // Generate the client key:
    /// let cks = ClientKey::new(PARAM_MESSAGE_2_CARRY_2, size);
    ///
    /// // Generate the server key:
    /// let sks = ServerKey::new(&cks);
    /// ```
    pub fn from_shortint(
        cks: &ClientKey,
        mut key: concrete_shortint::server_key::ServerKey,
    ) -> ServerKey {
        // It should remain just enough space add a carry
        let max =
            (cks.key.parameters.message_modulus.0 - 1) * cks.key.parameters.carry_modulus.0 - 1;

        key.max_degree = MaxDegree(max);
        ServerKey { key }
    }

    /// Create a ciphertext filled with zeros
    ///
    /// # Example
    ///
    /// ```rust
    /// use concrete_integer::gen_keys;
    /// use concrete_shortint::parameters::DEFAULT_PARAMETERS;
    ///
    /// let size = 4;
    ///
    /// // Generate the client key and the server key:
    /// let (cks, sks) = gen_keys(&DEFAULT_PARAMETERS, size);
    ///
    /// let ctxt = sks.create_trivial_zero(size, vec![], vec![]);
    ///
    /// // Decrypt:
    /// let dec = cks.decrypt(&ctxt);
    /// assert_eq!(0, dec);
    /// ```
    pub fn create_trivial_zero(
        &self,
        size: usize,
        message_modulus_vec: Vec<u64>,
        key_id_vec: Vec<KeyId>,
    ) -> Ciphertext {
        let mut vec_res = Vec::<concrete_shortint::Ciphertext>::with_capacity(size);
        for _ in 0..size {
            vec_res.push(self.key.create_trivial(0_u8));
        }

        Ciphertext {
            ct_vec: vec_res,
            message_modulus_vec,
            key_id_vec,
        }
    }

    /// Propagate the carry of the 'index' block to the next one.
    ///
    /// # Example
    ///
    ///```rust
    /// use concrete_integer::gen_keys;
    /// use concrete_shortint::parameters::PARAM_MESSAGE_2_CARRY_2;
    ///
    /// let size = 4;
    ///
    /// // Generate the client key and the server key:
    /// let (cks, sks) = gen_keys(&PARAM_MESSAGE_2_CARRY_2, size);
    ///
    /// let msg = 7;
    ///
    /// let ct1 = cks.encrypt(msg);
    /// let ct2 = cks.encrypt(msg);
    ///
    /// // Compute homomorphically an addition:
    /// let mut ct_res = sks.unchecked_add(&ct1, &ct2);
    /// sks.propagate(&mut ct_res, 0);
    ///
    /// // Decrypt one block:
    /// let res = cks.decrypt_one_block(&ct_res.blocks()[1]);
    /// assert_eq!(3, res);
    /// ```
    pub fn propagate(&self, ctxt: &mut Ciphertext, index: usize) {
        let carry = self.key.carry_extract(&ctxt.ct_vec[index]);

        ctxt.ct_vec[index] = self.key.message_extract(&ctxt.ct_vec[index]);

        //add the carry to the next block
        if index < ctxt.ct_vec.len() - 1 {
            self.key
                .unchecked_add_assign(&mut ctxt.ct_vec[index + 1], &carry);
        }
    }

    /// Propagate all the carries.
    ///
    /// # Example
    ///
    ///```rust
    /// use concrete_integer::gen_keys;
    /// use concrete_shortint::parameters::PARAM_MESSAGE_2_CARRY_2;
    ///
    /// let size = 4;
    ///
    /// // Generate the client key and the server key:
    /// let (cks, sks) = gen_keys(&PARAM_MESSAGE_2_CARRY_2, size);
    ///
    /// let msg = 10;
    ///
    /// let mut ct1 = cks.encrypt(msg);
    /// let mut ct2 = cks.encrypt(msg);
    ///
    /// // Compute homomorphically an addition:
    /// let mut ct_res = sks.unchecked_add(&mut ct1, &mut ct2);
    /// sks.full_propagate(&mut ct_res);
    ///
    /// // Decrypt:
    /// let res = cks.decrypt(&ct_res);
    /// assert_eq!(msg + msg, res);
    /// ```
    pub fn full_propagate(&self, ctxt: &mut Ciphertext) {
        let len = ctxt.ct_vec.len();
        for i in 0..len {
            self.propagate(ctxt, i);
        }
    }
}

// parallelized versions
impl ServerKey {
    /// Create a ciphertext filled with zeros
    ///
    /// # Example
    ///
    /// ```rust
    /// use concrete_integer::gen_keys;
    /// use concrete_shortint::parameters::DEFAULT_PARAMETERS;
    ///
    /// let size = 4;
    ///
    /// // Generate the client key and the server key:
    /// let (cks, sks) = gen_keys(&DEFAULT_PARAMETERS, size);
    ///
    /// let ctxt = sks.create_trivial_zero_parallelized(size, vec![], vec![]);
    ///
    /// // Decrypt:
    /// let dec = cks.decrypt(&ctxt);
    /// assert_eq!(0, dec);
    /// ```
    pub fn create_trivial_zero_parallelized(
        &self,
        size: usize,
        message_modulus_vec: Vec<u64>,
        key_id_vec: Vec<KeyId>,
    ) -> Ciphertext {
        self.create_trivial_zero(size, message_modulus_vec, key_id_vec)
    }

    /// Propagate the carry of the 'index' block to the next one.
    ///
    /// # Example
    ///
    ///```rust
    /// use concrete_integer::gen_keys;
    /// use concrete_shortint::parameters::PARAM_MESSAGE_2_CARRY_2;
    ///
    /// let size = 4;
    ///
    /// // Generate the client key and the server key:
    /// let (cks, sks) = gen_keys(&PARAM_MESSAGE_2_CARRY_2, size);
    ///
    /// let msg = 7;
    ///
    /// let ct1 = cks.encrypt(msg);
    /// let ct2 = cks.encrypt(msg);
    ///
    /// // Compute homomorphically an addition:
    /// let mut ct_res = sks.unchecked_add(&ct1, &ct2);
    /// sks.propagate_parallelized(&mut ct_res, 0);
    ///
    /// // Decrypt one block:
    /// let res = cks.decrypt_one_block(&ct_res.blocks()[1]);
    /// assert_eq!(3, res);
    /// ```
    pub fn propagate_parallelized(&self, ctxt: &mut Ciphertext, index: usize) {
        let (carry, message) = rayon::join(
            || self.key.carry_extract(&ctxt.ct_vec[index]),
            || self.key.message_extract(&ctxt.ct_vec[index]),
        );
        ctxt.ct_vec[index] = message;

        //add the carry to the next block
        if index < ctxt.ct_vec.len() - 1 {
            self.key
                .unchecked_add_assign(&mut ctxt.ct_vec[index + 1], &carry);
        }
    }

    /// Propagate all the carries.
    ///
    /// # Example
    ///
    ///```rust
    /// use concrete_integer::gen_keys;
    /// use concrete_shortint::parameters::PARAM_MESSAGE_2_CARRY_2;
    ///
    /// let size = 4;
    ///
    /// // Generate the client key and the server key:
    /// let (cks, sks) = gen_keys(&PARAM_MESSAGE_2_CARRY_2, size);
    ///
    /// let msg = 10;
    ///
    /// let mut ct1 = cks.encrypt(msg);
    /// let mut ct2 = cks.encrypt(msg);
    ///
    /// // Compute homomorphically an addition:
    /// let mut ct_res = sks.unchecked_add(&mut ct1, &mut ct2);
    /// sks.full_propagate_parallelized(&mut ct_res);
    ///
    /// // Decrypt:
    /// let res = cks.decrypt(&ct_res);
    /// assert_eq!(msg + msg, res);
    /// ```
    pub fn full_propagate_parallelized(&self, ctxt: &mut Ciphertext) {
        let len = ctxt.ct_vec.len();
        for i in 0..len {
            self.propagate_parallelized(ctxt, i);
        }
    }
}
