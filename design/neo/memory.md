scope -> value -> atom -> base

scope -> O2M -> value

value -> M2O -> atom

atom -> M2O -> base (reference count)



scope -> delete -> value

value -> gc -> atom

atom -> dispose -> base

base -> delete -> base