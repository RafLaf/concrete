use crate::ClientKey;

/// Trait used to have a generic way of creating a value of a FHE type
/// from a native value.
///
/// This trait is for when FHE type the native value is encrypted
/// supports the same numbers of bits of precision.
///
/// The `ClientKey` is required as it contains the key needed to do the
/// actual encryption.
pub trait FheEncrypt<T> {
    // We avoid naming it `from` as it clashes too much with `std::convert::From`
    fn encrypt(value: T, key: &ClientKey) -> Self;
}

pub trait DynamicFheEncryptor<T> {
    type FheType;

    fn encrypt(&self, value: T, key: &ClientKey) -> Self::FheType;
}

// This trait has the same signature than
// `std::convert::From` however we create our own trait
// to be explicit about the `trivial`
pub trait FheTrivialEncrypt<T> {
    fn encrypt_trivial(value: T) -> Self;
}

pub trait DynamicFheTrivialEncryptor<T> {
    type FheType;

    fn encrypt_trivial(&self, value: T) -> Self::FheType;
}

/// Trait used to have a generic **fallible** way of creating a value of a FHE type.
///
/// For example this trait may be implemented by FHE types which may not be able
/// to represent all the values of even the smallest native type.
///
/// For example, `FheUint2` which has 2 bits of precision may not be constructed from
/// all values that a `u8` can hold.
pub trait FheTryEncrypt<T>
where
    Self: Sized,
{
    type Error: std::error::Error;

    // We avoid naming it `try_from` as it clashes too much with `std::convert::TryFrom`
    fn try_encrypt(value: T, key: &ClientKey) -> Result<Self, Self::Error>;
}

pub trait DynamicFheTryEncryptor<T> {
    type FheType;
    type Error;

    fn try_encrypt(&self, value: T, key: &ClientKey) -> Result<Self::FheType, Self::Error>;
}

/// Decrypt a FHE type to a native type.
pub trait FheDecrypt<T> {
    fn decrypt(&self, key: &ClientKey) -> T;
}

/// Trait required to apply univariate function over homomorphic types.
///
/// A `univariate function` is a function with one variable, e.g., of the form f(x).
pub trait FheBootstrap
where
    Self: Sized,
{
    /// Compute a function over an encrypted message, and returns a new encrypted value containing
    /// the result.
    fn map<F: Fn(u64) -> u64>(&self, func: F) -> Self;

    /// Compute a function over the encrypted message.
    fn apply<F: Fn(u64) -> u64>(&mut self, func: F);
}

#[doc(hidden)]
pub trait FheNumberConstant {
    const MIN: u64;
    const MAX: u64;
    const MODULUS: u64;
}
