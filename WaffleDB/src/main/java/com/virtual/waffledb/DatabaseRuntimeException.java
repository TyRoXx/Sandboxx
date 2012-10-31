package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public class DatabaseRuntimeException extends RuntimeException {
    public DatabaseRuntimeException(String message) {
        super(message);
    }
    
    public void rethrow() throws DatabaseException {
        throw new DatabaseException(getMessage());
    }
}
